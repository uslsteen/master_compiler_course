#pragma once

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
//
#include "ilist_base.hh"

namespace jj_ir::ilist_detail {
//

template <typename Type, bool Cond>
using MakeConstType = std::conditional_t<Cond, std::add_const_t<Type>, Type>;

/**
 * @brief Iterator for intrusive lists  based on ilist_node.
 *        Implementation without iterator traits
 *        To check out correctness of INodeType tparam.
 *
 * @tparam INodeType,
 * @tparam IsConst
 * @tparam IsReverse
 */
template <typename INodeType, bool IsConst, bool IsReverse>
class ilist_iterator final {
public:
    using value_type = MakeConstType<INodeType, IsConst>;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using node_pointer = MakeConstType<ilist_node, IsConst>*;

private:
    node_pointer m_ptr = nullptr;

public:
    ilist_iterator() = default;

    /**
     * @brief Construct from a ilist_node
     */
    explicit ilist_iterator(node_pointer node) : m_ptr(node) {}

    explicit ilist_iterator(pointer pnode) : m_ptr(pnode) {}

    explicit ilist_iterator(reference node_ref) : m_ptr(&node_ref) {}

    //! TODO:  explicit conversion forward/reverse iterator
    //! TODO:  const - cast
    //! TOOD:  ctor a const iterator from a nonconst iterator

    //! NOTE: Accessors

    reference operator*() const noexcept {
        return *static_cast<pointer>(m_ptr);
    }

    pointer operator->() const noexcept { return &operator*(); }

    //! NOTE: Comparision
    friend bool operator==(const ilist_iterator& lhs,
                           const ilist_iterator rhs) noexcept {
        return lhs.m_ptr == rhs.m_ptr;
    }

    friend bool operator!=(const ilist_iterator& lhs,
                           const ilist_iterator rhs) noexcept {
        return !(lhs == rhs);
    }

    //! NOTE: Increment, decrement operators
    ilist_iterator& operator++() noexcept {
        m_ptr = IsReverse ? m_ptr->get_prev() : m_ptr->get_next();
        return *this;
    }

    ilist_iterator& operator--() noexcept {
        m_ptr = IsReverse ? m_ptr->get_next() : m_ptr->get_prev();
        return *this;
    }

    ilist_iterator operator--(int) {
        auto tmp = *this;
        --*this;
        return tmp;
    }
    ilist_iterator operator++(int) {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    node_pointer get_pnode() const noexcept {
        return static_cast<node_pointer>(m_ptr);
    }
};

}  // namespace jj_ir::ilist_detail