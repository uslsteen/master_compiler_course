#pragma once

#include "graph/dfs.hh"
#include "graph/dom3.hh"
#include "loop_base.hh"
//
#include "unordered_map"

namespace jj_vm::analysis::loop {

template <class GraphTy>
class LoopTree final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using const_node_pointer = typename GraphTy::const_node_pointer;

    LoopTree() = default;

private:
    //! NOTE: rename all internal data as m_data
    std::unordered_map<const_node_pointer, Loop3NodeBase<GraphTy>> m_data{};
};

template <class GraphTy>
class LoopTreeBuilder final {
public:
    using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using const_node_pointer = typename GraphTy::const_node_pointer;
    //

private:
    //
    jj_vm::graph::dom3_impl::DomTree<GraphTy> m_dom3{};
    std::vector<const_node_pointer> m_dfs_nodes{};
    //
    LoopTree<GraphTy> m_tree{};
    //
    LoopTreeBuilder(const GraphTy& graph) {
        //
    }

private:

    void collect_back_edges() {}
    void populate_loops() {}
    //
    auto tree() const noexcept { return m_tree; }

public:
    static auto build(const GraphTy &graph) {
        LoopTreeBuilder<GraphTy> builder{graph};
        return builder.tree();
    }
};

}  // namespace jj_vm::analysis::loop