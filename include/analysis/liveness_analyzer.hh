#pragma once

#include "IR/basic_block.hh"
#include "graph/dfs.hh"
#include "graph/dom3.hh"
#include "loop_analyzer.hh"
#include "linear_order.hh"
//
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

namespace jj_vm::analysis::liveness {

template <typename GraphTy>
class LivenessAnalyzer final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using const_node_pointer = typename GraphTy::const_node_pointer;
    //
    using OrderTy = typename LinearOrderBuilder<GraphTy>::OrderTy;

private:
    OrderTy m_linear_order{};
    loop::LoopTree<GraphTy> m_loop_tree{};

public:
    LivenessAnalyzer(const GraphTy& graph) {
        m_loop_tree =
            jj_vm::analysis::loop::LoopTreeBuilder<GraphTy>::build(graph);
        m_linear_order = jj_vm::analysis::order::LinearOrderBuilder<GraphTy>::build(graph, m_loop_tree);

        assert(
            graph.size() == m_linear_order.size() &&
            "Error : few basic blocks were dropped due linear order building");
    }

    auto linear_order() const { return m_linear_order; }
};

}  // namespace jj_vm::analysis::liveness
