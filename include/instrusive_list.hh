#ifndef INSTRUSIVE_LIST_HH
#define INSTRUSIVE_LIST_HH

#include <memory>
#include <utility>
#include <vector>

namespace jj_ir {
//

template <typename T>
class INode {
protected:
    T* m_next = nullptr;
    T* m_prev = nullptr;

public:
    INode() = default;

    INode(const INode&) = delete;
    INode& operator=(const INode&) = delete;

    INode(INode&&) = delete;
    INode& operator=(INode&&) = delete;

    ~INode() = default;

    T* next() const { return m_next; }
    T* prev() const { return m_prev; }

    void insert_next(INode* cur) {}
    void insert_prev(INode* cur) {}
};

template <typename ListNodeT>
class IntrusiveList {
    std::vector<std::unique_ptr<ListNodeT>> m_internal{};

public:
    template <class... Args>
    void emplace_back(Args&&... args) {
        const ListNodeT* p_inserted = m_internal.emplace_back(
            std::make_unique<ListNodeT>(std::forward<Args>(args)...));

        //! NOTE: check the first item of list
        if (!m_internal.empty())
            p_inserted->insert_next(m_internal.back().get());
    }

    auto size() const noexcept { return m_internal.size(); }

    auto empty() const noexcept { return m_internal.empty(); }
};

}  // namespace jj_ir

#endif  // INSTRUSIVE_LIST_HH