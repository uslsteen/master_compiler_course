#pragma once

#include <cassert>

namespace jj_vm::ilist_detail {

/**
 * @brief Base class for ilist nodes.
 */
class ilist_node {
    //
    ilist_node *m_prev = nullptr;
    ilist_node *m_next = nullptr;
    //
public:
    //
    ilist_node() = default;
    //
    ilist_node(const ilist_node &) = delete;
    ilist_node &operator=(const ilist_node &) = delete;
    //
    ilist_node(ilist_node &) = delete;
    ilist_node &operator=(ilist_node &&) = delete;
    //
    void set_prev(ilist_node *prev) noexcept { m_prev = prev; }
    void set_next(ilist_node *next) noexcept { m_next = next; }
    //
    auto *get_prev() const noexcept { return m_prev; }
    auto *get_next() const noexcept { return m_next; }
};

/**
 * @brief Implementation of list algorithms using ilist_node.
 */
class ilist_base {
public:
    using node_base_type = ilist_node;

protected:
    //
    static void insert_before(node_base_type *next,
                              node_base_type *to_insert) noexcept {
        assert(next != nullptr && "Next base node is nullptr");
        assert(to_insert != nullptr &&
               "Node which should be inserted is nullptr");

        auto *prev = next->get_prev();
        if (prev != nullptr)
            prev->set_next(to_insert);
        //
        to_insert->set_prev(prev);
        to_insert->set_next(next);
        //
        next->set_prev(to_insert);
    }

    //
    static void remove(node_base_type *to_remove) noexcept {
        auto *next = to_remove->get_next();
        auto *prev = to_remove->get_prev();
        //
        next->set_prev(prev);
        prev->set_next(next);

        //! NOTE: Not necessary, but helps catch a class of bugs.
        to_remove->set_prev(nullptr);
        to_remove->set_next(nullptr);
    }

    //

    static void move_before(node_base_type *next, node_base_type *first,
                            node_base_type *last) {
        assert(next != nullptr && "Error: next node is nullptr");
        assert(first != nullptr && "Error: first node is nullptr");
        assert(last != nullptr && "Error: last node is nullptr");

        assert(next != first && "Error: unable to insert last before first");

        if (last == next || first == last)
            return;

        auto *const to_insert = last->get_prev();
        auto *const rbegin = first->get_prev();
        last->set_prev(rbegin);
        rbegin->set_next(last);

        auto *const prev = next->get_prev();
        next->set_prev(to_insert);
        prev->set_next(first);

        to_insert->set_next(next);
        first->set_prev(prev);
    }
};

}  // namespace jj_vm::ilist_detail
