#pragma once

#include <vector>

namespace jj_vm::graph::dom3_impl {
//

template <class NodeTy>
class Dom3NodeBase {
    //
public:
    using node_pointer = NodeTy*;
    // using const_node_pointer = const NodeTy*;

private:
    //! NOTE: generally there is should be pointer to Dom3NodeBase as iDom
    //        but right now you can see temporary solution

    //! NOTE: immediate dominator == idom
    node_pointer m_idom{nullptr};
    std::vector<node_pointer> m_idommed{};
    //
    Dom3NodeBase* m_idom_node{nullptr};

public:
    //
    Dom3NodeBase() = default;
    Dom3NodeBase(node_pointer pnode,
                 const std::vector<node_pointer>& dommed)
        : m_idom(pnode), m_idommed(dommed) {}

    explicit Dom3NodeBase(node_pointer pnode) : m_idom(pnode) {}

    void set_idom(node_pointer pnode) const noexcept { m_idom = pnode; }

    auto idom() const noexcept { return m_idom; }
    auto idommed() const noexcept { return m_idommed; }

    auto push_idom(node_pointer pnode) { m_idommed.push_back(pnode); }
};

}  // namespace jj_vm::graph::dom3_impl
