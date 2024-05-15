#pragma once

#include "liveness_analyzer.hh"
//
#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <ostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jj_vm::analysis::regalloc {

using RegIdTy = std::size_t;

struct Location {
    RegIdTy id;
    bool on_stack;
};

bool operator==(const Location& lhs, const Location& rhs) {
    return lhs.id == rhs.id && lhs.on_stack == rhs.on_stack;
}

std::ostream& operator<<(std::ostream& in, const Location& data) {
    return in << (data.on_stack ? "r" : "s") << "=" << data.id;
}

template <typename GraphTy, RegIdTy RegsNum>
class RegAllocAnalyzer final {
public:
    using LiveIntevalTy = typename jj_vm::analysis::liveness::LivenessAnalyzer<
        GraphTy>::LiveIntevalTy;

private:
    jj_vm::analysis::liveness::LivenessAnalyzer<GraphTy> m_liveness;
    //
    std::vector<RegIdTy> m_reg_pool;
    std::unordered_map<jj_vm::ir::Value*, Location> m_regmap;
    std::unordered_map<LiveIntevalTy*, jj_vm::ir::Value*> m_range2val;
    //
    std::vector<std::pair<jj_vm::ir::Value*, LiveIntevalTy*>> m_data;
    std::set<LiveIntevalTy*, jj_vm::ir::ComparePEnd> m_active;
    //
    RegIdTy m_stack_loc{0};

public:
    RegAllocAnalyzer() = default;

    RegAllocAnalyzer(const RegAllocAnalyzer&) = delete;
    RegAllocAnalyzer(RegAllocAnalyzer&&) = delete;

    RegAllocAnalyzer& operator=(const RegAllocAnalyzer&) = delete;
    RegAllocAnalyzer& operator=(RegAllocAnalyzer&&) = delete;

    RegAllocAnalyzer(const GraphTy& graph)
        : m_liveness(jj_vm::analysis::liveness::LivenessBuilder<GraphTy>::build(
              graph)) {
        m_reg_pool.resize(RegsNum);
        std::iota(m_reg_pool.rbegin(), m_reg_pool.rend(), 0);
        //
        m_data.reserve(m_liveness.intervals().size());

        auto& life_intervals = m_liveness.intervals();

        for (auto&& [val, interval] : life_intervals) {
            auto* pinterval = &interval;
            if (interval.begin() != interval.end()) {
                m_data.push_back(std::make_pair(val, pinterval));
                m_range2val[pinterval] = val;
            }
        }

        //! NOTE: getting order the life ranges
        std::sort(
            m_data.begin(), m_data.end(), [](const auto& lhs, const auto& rhs) {
                auto lbeg = lhs.second->begin(), rbeg = rhs.second->begin();
                auto lend = lhs.second->end(), rend = rhs.second->end();
                if (lbeg == rbeg) {
                    if (lend == rbeg) return lhs.first < rhs.first;
                    return lend < rend;
                }
                return lbeg < rbeg;
            });
    }

public:
    void linear_scan() {
        for (auto&& [val, cur_interval] : m_data) {
            const auto* instr = static_cast<const jj_vm::ir::Instr*>(val);
            expire_old_intervals(cur_interval);

            if (m_active.size() == RegsNum)
                spill_at_inverval(cur_interval, val);
            else {
                auto free_reg = m_reg_pool.back();
                m_reg_pool.pop_back();
                //
                m_active.insert(cur_interval);
                m_regmap[val] = Location{free_reg, false};
            }
        }
    }

    auto get_location(jj_vm::ir::Value* val) const { return m_regmap.at(val); }

private:
    void expire_old_intervals(LiveIntevalTy* cur_interval) {
        for (auto&& active_it = m_active.begin();
             active_it != m_active.end();) {
            if (cur_interval->begin() < (*active_it)->end()) return;

            Location& free_reg = m_regmap[m_range2val[*active_it]];
            m_reg_pool.push_back(free_reg.id);

            active_it = m_active.erase(active_it);
        }
    }

    void spill_at_inverval(LiveIntevalTy* cur_interval,
                           jj_vm::ir::Value* cur_val) {
        LiveIntevalTy* spill_interval = *m_active.rbegin();
        auto&& cur_loc = m_regmap[cur_val];

        if (cur_interval->end() < spill_interval->end()) {
            jj_vm::ir::Value* spill_val = m_range2val[spill_interval];
            auto&& spill_loc = m_regmap[spill_val];
            //
            cur_loc.id = spill_loc.id;

            spill_loc.id = m_stack_loc;
            spill_loc.on_stack = true;

            m_active.erase(spill_interval);
            m_active.insert(cur_interval);
        } else
            cur_loc = Location{m_stack_loc, true};

        ++m_stack_loc;
    }
};
}  // namespace jj_vm::analysis::regalloc
