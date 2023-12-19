#pragma once

namespace jj_vm::analysis::loop {
//

template <class GraphTy>
class LoopNodeBase {
public:
    // using value_type = typename GraphTy::value_type;
    using node_pointer = typename GraphTy::node_pointer;
    using const_node_pointer = typename GraphTy::const_node_pointer;

private:
    const_node_pointer m_header = nullptr;
    bool m_reducible = false;

    //! NOTE: root loop contains blocks, which are not assigned to any loops
    bool m_root = false;
    //
    std::set<const_node_pointer> m_loop_body{};
    std::set<const_node_pointer> m_back_edges{};
    std::set<const LoopNodeBase<GraphTy>*> m_inners{};
    //
    LoopNodeBase<GraphTy>* m_outer = nullptr;
    //
public:
    LoopNodeBase(const_node_pointer pnode, bool is_reducible = false,
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
    void add_node(const_node_pointer node) { m_loop_body.insert(node); }
    //
    void add_inner(LoopNodeBase<GraphTy>* inner) {
        m_inners.insert(inner);
        inner->set_outer(this);
    }
    //
    void add_back_edge(const_node_pointer node) {
        m_back_edges.insert(node);
        add_node(node);
    }
};

}  // namespace jj_vm::analysis::loop