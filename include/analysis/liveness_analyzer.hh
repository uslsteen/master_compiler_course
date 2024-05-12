#pragma once

#include "IR/basic_block.hh"
#include "IR/instruction.hh"
#include "linear_order.hh"
#include "loop_analyzer.hh"
//
#include <cassert>
#include <unordered_map>
#include <unordered_set>

namespace jj_vm::analysis::liveness {

template <typename GraphTy>
class LivenessBuilder;

class LiveInterval final {
    size_t m_begin{};
    size_t m_end{};

public:
    constexpr LiveInterval() = default;
    constexpr LiveInterval(size_t begin, size_t end)
        : m_begin(begin), m_end(end) {
        assert(m_begin > m_end && "Error: invalid interval, begin > end");
    }
    //

    auto begin() const noexcept { return m_begin; }
    auto end() const noexcept { return m_end; }
};

template <typename GraphTy>
class LivenessAnalyzer final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    // using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    using OrderTy = typename jj_ir::analysis::order::LinearOrderBuilder<GraphTy>::OrderTy;
    using LiveSetTy = std::unordered_set<ir::Instr*>;

private:
    friend LivenessBuilder<GraphTy>;

    std::unordered_map<ir::Value*, LiveInterval> m_intervals;
    std::unordered_map<node_pointer, LiveSetTy> m_live_sets;
    //
public:
    LivenessAnalyzer() = default;

    LivenessAnalyzer(const LivenessAnalyzer&) = default;
    LivenessAnalyzer(LivenessAnalyzer&&) = default;

    LivenessAnalyzer& operator=(const LivenessAnalyzer&) = default;
    LivenessAnalyzer& operator=(LivenessAnalyzer&&) = default;

    //
    auto intervals() const noexcept { return m_intervals; }

    //
    auto live_sets() const noexcept { return m_live_sets; }

public:
};

template <typename GraphTy>
class LivenessBuilder {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    // using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    using OrderTy = typename LivenessAnalyzer<GraphTy>::OrderTy;
    using LiveSetTy = typename LivenessAnalyzer<GraphTy>::LiveSetTy;

private:
    loop::LoopTree<GraphTy> m_loop_tree{};
    OrderTy m_linear_order{};

    std::unordered_map<ir::Value*, LiveInterval> m_intervals;
    std::unordered_map<node_pointer, LiveSetTy> m_live_sets;

public:
    LivenessBuilder(const GraphTy& graph) {
        m_loop_tree =
            jj_vm::analysis::loop::LoopTreeBuilder<GraphTy>::build(graph);
        m_linear_order =
            jj_ir::analysis::order::LinearOrderBuilder<GraphTy>::build(
                graph, m_loop_tree);
        //
        build_intervals();
    }

private:
    /**
     * @brief
     *
     */
    void init_live_values() {
        //
    }

    /**
     * @brief
     *
     */
    void calc_life_ranges() {
        //
    }

    /**
     * @brief Function to build live intervals:
                    1. assign live numbers for each instruction
                    2. calucate life ranges (algorithm from lectures)
     *
     */
    void build_intervals() {
        init_live_values();
        calc_life_ranges();
    }

public:
    static LivenessAnalyzer<GraphTy> build(const GraphTy& graph) {
        LivenessBuilder<GraphTy> builder{graph};
        LivenessAnalyzer<GraphTy> analyzer{};
        //
        analyzer.m_intervals = std::move(builder.m_intervals);
        analyzer.m_live_sets = std::move(builder.m_live_sets);

        return analyzer;
    }
};

}  // namespace jj_vm::analysis::liveness
