#pragma once

#include <set>
#include <vector>

namespace jj_vm::analysis::loop {
//
template <class GraphTy>
class LoopNodeBase {
public:
    using node_pointer = typename GraphTy::node_pointer;
    // using const_node_pointer = typename GraphTy::const_node_pointer;
    using iterator = typename std::vector<node_pointer>::iterator;
    using reverse_iterator = typename std::vector<node_pointer>::reverse_iterator;
    using const_iterator = typename std::vector<node_pointer>::const_iterator;
    using const_reverse_iterator = typename std::vector<node_pointer>::const_reverse_iterator;

private:
    node_pointer m_header = nullptr;
    bool m_reducible = false;

    //! NOTE: root loop contains blocks, which are not assigned to any loops
    bool m_root = false;
    //
    std::vector<node_pointer> m_loop_body;
    std::set<node_pointer> m_back_edges;
    std::set<const LoopNodeBase<GraphTy>*> m_inners;
    //
    LoopNodeBase<GraphTy>* m_outer = nullptr;
    //
public:
    LoopNodeBase() = default;

    LoopNodeBase(node_pointer pnode, bool is_reducible = false,
                 bool root = false)
        : m_header(pnode), m_reducible(is_reducible), m_root(root) {}

    //
    auto header() const noexcept { return m_header; }
    //
    auto is_reducible() const noexcept { return m_reducible; }
    //
    auto is_root() const noexcept { return m_root; }
    //
    auto loop_body() const noexcept { return m_loop_body; }
    //
    auto back_edges() const noexcept { return m_back_edges; }
    //
    auto inners() const noexcept { return m_inners; }
    //
    auto outer() const noexcept { return m_outer; }

public:
    void set_outer(LoopNodeBase<GraphTy>* node) { m_outer = node; }
    //
    void add_node(node_pointer node) { m_loop_body.push_back(node); }
    //
    void add_inner(LoopNodeBase<GraphTy>* inner) {
        m_inners.insert(inner);
        inner->set_outer(this);
    }
    //
    void add_back_edge(node_pointer node) {
        m_back_edges.insert(node);
        add_node(node);
    }

    iterator begin() { return m_loop_body.begin(); }
    const_iterator begin() const { return m_loop_body.begin(); }
    reverse_iterator rbegin() { return m_loop_body.rbegin(); }
    const_reverse_iterator rbegin() const { return m_loop_body.rbegin(); }

    iterator end() { return m_loop_body.end(); }
    const_iterator end() const { return m_loop_body.end(); }
    reverse_iterator rend() { return m_loop_body.rend(); }
    const_reverse_iterator rend() const { return m_loop_body.rend(); }
};
}  // namespace jj_vm::analysis::loop
