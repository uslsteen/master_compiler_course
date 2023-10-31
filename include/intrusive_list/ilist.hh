#pragma once

#include <iterator>

#include "ilist_base.hh"
#include "ilist_iterator.hh"
#include "ilist_node.hh"

namespace jj_ir {
namespace ilist_detail {

class ilist_sentinel : public ilist_detail::ilist_node {
public:
    //
    ilist_sentinel() { reset(); }

    //
    void reset() {
        set_prev(this);
        set_next(this);
    }

    //
    bool empty() const noexcept { return this == get_prev(); }
};

/**
 * @brief Use delete by default for ilist_impl
 *
 * @tparam NodeTy
 */
template <typename NodeTy>
struct ilist_alloca_traits {
    static void dealloc(NodeTy *node) { delete node; }
};

/**
 * @brief Custom traits to do nothing on deletion.
 *
 * @tparam NodeTy
 */
template <typename NodeTy>
struct ilist_noalloca_traits {
    static void dealloc(NodeTy *node) {}
};

/**
 * @brief Simple intrusive list implementation
 *        This intrusive takes ownership of anything insertet in it according
 *        AllocaTraits policy
 *
 * @tparam NodeTy - instrusive list node type
 * @tparam AllocaTraits
 */
template <class NodeTy, class AllocaTraits>
class ilist_impl final : public ilist_detail::ilist_base, public AllocaTraits {
public:
    using iterator = ilist_detail::ilist_iterator<NodeTy, false, false>;
    using const_iterator = ilist_detail::ilist_iterator<NodeTy, true, false>;
    using reverse_iterator = ilist_detail::ilist_iterator<NodeTy, false, true>;
    using const_reverse_iterator =
        ilist_detail::ilist_iterator<NodeTy, true, true>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = typename iterator::value_type;
    using pointer = typename iterator::pointer;
    using reference = typename iterator::reference;
    using const_pointer = typename const_iterator::pointer;
    using const_reference = typename const_iterator::reference;
    //
private:
    ilist_sentinel m_internal{};

public:
    ilist_impl() = default;

    //! NOTE: no copy semantic
    ilist_impl(const ilist_impl &) = delete;
    ilist_impl &operator=(const ilist_impl &) = delete;

    //! NOTE: trivial move semantic.
    ilist_impl(ilist_impl &&other) { splice(end(), other); }

    ilist_impl &operator=(ilist_impl &&other) {
        clear();
        splice(end(), other);
        return *this;
    }

    ~ilist_impl() { clear(); }

    //! NOTE: iterators
    iterator begin() noexcept { return ++iterator{&m_internal}; }
    const_iterator begin() const noexcept {
        return ++const_iterator{&m_internal};
    }

    iterator end() noexcept { return iterator{&m_internal}; }
    const_iterator end() const noexcept { return const_iterator{&m_internal}; }

    //! NOTE: O(1)
    bool empty() const noexcept { return m_internal.empty(); }

    //! NOTE: O(n)
    size_type size() const noexcept { return std::distance(begin(), end()); }

    //! NOTE: Accessors
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    //
    reference back() { return *std::prev(end()); }
    const_reference back() const { return *std::prev(end()); }

    //! NOTE: Modify list
    iterator insert(iterator pos, pointer node) {
        ilist_detail::ilist_base::insert_before(pos.get_pnode(), node);
        return iterator{node};
    }

    pointer remove(iterator pos) noexcept {
        pointer next_pnode = &*pos++;
        ilist_detail::ilist_base::remove(next_pnode);
        return next_pnode;
    }

    iterator erase(iterator pos) noexcept {
        assert(pos != end() && "Cannot remove end()");
        //
        AllocaTraits::dealloc(remove(pos));
        return pos;
    }

    iterator erase(iterator first, iterator last) noexcept {
        while (first != last) first = erase(first);
        //
        return first;
    }

    void push_front(pointer node) { insert(begin(), node); }
    void push_back(pointer node) { insert(end(), node); }

    //! NOTE: Splice in another list
    void splice(iterator pos, ilist_impl &other) noexcept {
        splice(pos, other.begin(), other.end());
    }

    //! NOTE: Splice in a node from another list.
    void splice(iterator pos, iterator it) noexcept {
        splice(pos, it, std::next(it));
    }

    /// Splice in a range of nodes from another list.
    void splice(iterator pos, iterator first, iterator last) noexcept {
        ilist_detail::ilist_base::move_before(
            *pos.get_pnode(), *first.get_pnode(), *last.get_pnode());
    }

    void clear() { erase(begin(), end()); };
};

}  // namespace ilist_detail
/**
 * @brief
 *
 * @tparam T
 */
template <class T>
using ilist = ilist_detail::ilist_impl<T, ilist_detail::ilist_alloca_traits<T>>;

template <class T>
using ilist_view =
    ilist_detail::ilist_impl<T, ilist_detail::ilist_noalloca_traits<T>>;

template <class T, class... Args, class NodeTy>
auto &emplace_back(ilist<NodeTy>& ilist, Args &&...args) {
    auto *to_emplace = new T{std::forward<Args>(args)...};
    //
    ilist.push_back(to_emplace);
    return *to_emplace;
}
}  // namespace jj_ir