#pragma once

#include "liveness_analyzer.hh"
//
#include <algorithm>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <vector>

namespace jj_vm::analysis::regalloc {

template <typename GraphTy, std::size_t RegsNum>
class RegAllocAnalyzer final {
public:
    using LiveIntevalTy = typename jj_vm::analysis::liveness::LivenessAnalyzer<
        GraphTy>::LiveIntevalTy;
    //
public:
    RegAllocAnalyzer() = default;

    //
    RegAllocAnalyzer(const GraphTy& graph)
        : m_liveness(jj_vm::analysis::liveness::LivenessBuilder<GraphTy>::build(
              graph)) {
        m_reg_pool.resize(RegsNum);
        std::iota(m_reg_pool.begin(), m_reg_pool.end(), 0);
        //
        m_intervals.reserve(m_liveness.intervals().size());
        m_instrs.reserve(m_liveness.intervals().size());
        //
        for (auto&& [val, interval] : m_liveness.intervals()) {
            m_intervals.push_back(interval);
            m_instrs.push_back(val);
        }
    }

private:
public:
    void run_scan() {}

private:
    jj_vm::analysis::liveness::LivenessAnalyzer<GraphTy> m_liveness;
    //
    std::vector<std::size_t> m_reg_pool;
    std::vector<LiveIntevalTy> m_intervals;
    std::vector<jj_vm::ir::Value*> m_instrs;
};
}  // namespace jj_vm::analysis::regalloc
