#pragma once

#include "IR/basic_block.hh"
#include "graph/dfs.hh"
#include "graph/dom3.hh"
#include "loop_analyzer.hh"
//
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

namespace jj_ir::analysis::order {

template <typename GraphTy>
class LinearOrderBuilder final {
public:
    using LoopTreeTy = typename jj_vm::analysis::loop::LoopTree<GraphTy>;
    //
    using node_pointer = typename GraphTy::node_pointer;
    using loop_base_pointer = typename LoopTreeTy::loop_base_pointer;
    using OrderTy = std::vector<node_pointer>;
    //
private:
    OrderTy m_rpo{};
    OrderTy m_linear_order{};

    LinearOrderBuilder() = default;

    LinearOrderBuilder(const LinearOrderBuilder&) = delete;
    LinearOrderBuilder(LinearOrderBuilder&&) = delete;

    LinearOrderBuilder& operator=(const LinearOrderBuilder&) = delete;
    LinearOrderBuilder& operator=(LinearOrderBuilder&&) = delete;

    explicit LinearOrderBuilder(const GraphTy& graph,
                                const LoopTreeTy& loop_tree)
        : m_rpo{jj_vm::graph::deep_first_search_reverse_postoder(graph)} {
        linearize_graph(loop_tree);
    }

    void linearize_graph(const LoopTreeTy& loop_tree) {
        std::set<node_pointer> visited;

        for (auto&& pnode : m_rpo) {
            if (visited.find(pnode) != visited.end()) continue;

            visited.insert(pnode);
            m_linear_order.push_back(pnode);

            auto* loop = loop_tree.get_loop(pnode);
            if (loop->header() == pnode && loop->is_reducible())
                linearize_loop(loop, loop_tree, visited);
        }
    }

    void linearize_loop(loop_base_pointer cur_loop, const LoopTreeTy& loop_tree,
                        std::set<node_pointer>& visited) {
        for (auto it = cur_loop->rbegin(); it != cur_loop->rend(); ++it) {
            auto loop_node = *it;
            //
            if (visited.find(loop_node) != visited.end()) continue;
            auto ploop_base = loop_tree.get_loop(loop_node);

            //! NOTE: process all inners loops
            if (ploop_base != cur_loop && ploop_base->header() == loop_node) {
                linearize_loop(ploop_base, loop_tree, visited);
                continue;
            }
            visited.insert(loop_node);
            m_linear_order.push_back(loop_node);
        }
    }

public:
    //
    static OrderTy build(const GraphTy& graph, const LoopTreeTy& loop_tree) {
        LinearOrderBuilder<GraphTy> builder{graph, loop_tree};
        return builder.m_linear_order;
    }
};
}  // namespace jj_ir::analysis::order
