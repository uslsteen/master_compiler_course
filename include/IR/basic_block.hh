#pragma once

#include <cassert>
#include <cstddef>
//
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>
//
#include "instruction.hh"
#include "instructions.hh"
#include "intrusive_list/ilist.hh"

namespace jj_vm::ir {

class Function;

/**
 * @brief Linear continious liveness interval
 */
class LiveInterval final {
    std::size_t m_begin{};
    std::size_t m_end{};
    //
public:
    LiveInterval() = default;
    LiveInterval(std::size_t begin, std::size_t end)
        : m_begin(begin), m_end(end) {
        // assert(m_begin > m_end && "Error: invalid interval, begin > end");
    }

    auto begin() const noexcept { return m_begin; }
    auto end() const noexcept { return m_end; }
    //
    void set_begin(std::size_t begin) { m_begin = begin; }
    void set_end(std::size_t end) { m_end = end; }
    //
    void update(const LiveInterval& other) {
        m_begin = std::min(other.m_begin, m_begin);
        m_end = std::max(other.m_end, m_end);
    }
    //
    bool is_equal(const LiveInterval& other) const noexcept {
        return m_begin == other.m_begin && m_end == other.m_end;
    }
};

struct ComparePEnd {
    bool operator()(const LiveInterval* lhs, const LiveInterval* rhs) const {
        return lhs->end() < rhs->end();
    }
};

bool operator==(const LiveInterval& lhs, const LiveInterval& rhs) {
    return lhs.is_equal(rhs);
}

bool operator!=(const LiveInterval& lhs, const LiveInterval& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& in, const LiveInterval& data) {
    return in << "(" << data.begin() << ", " << data.end() << ")";
}

/**
 * @brief Linear part of the code
 *
 */
class BasicBlock final : public ilist_detail::ilist_node {
public:
    using InstrList = ilist<Instr>;
    //
    using iterator = InstrList::iterator;
    using const_iterator = InstrList::const_iterator;
    using reverse_iterator = InstrList::reverse_iterator;
    using const_reverse_iterator = InstrList::const_reverse_iterator;
    //
    using id_type = uint32_t;
    //
private:
    id_type m_bb_id = 0;
    InstrList m_instr{};
    //
    std::vector<BasicBlock*> m_preds{};
    std::vector<BasicBlock*> m_succs{};
    //
    Function* m_parent = nullptr;
    //
    LiveInterval m_interval;
    //

    //! NOTE: it should be hidden in private due access to private data & dumb
    //!       func naming
    void add_succ(BasicBlock* succ) noexcept { m_succs.push_back(succ); }
    void add_pred(BasicBlock* pred) noexcept { m_preds.push_back(pred); }

    //
public:
    BasicBlock() = default;
    explicit BasicBlock(id_type bb_id, Function* parent = nullptr)
        : m_bb_id(bb_id), m_parent(parent) {}

    /**
     * @brief Creates a new basic block
     *
     * @param[in] Parent
     * @param[in] InsertBefore
     * @return BasicBlock*
     */
#if 0
    // TODO: implement it
    static BasicBlock* create(Function* Parent = nullptr,
                              BasicBlock* InsertBefore = nullptr) {
        return new BasicBlock{Parent, InsertBefore};
    }
#endif

    static void link_blocks(BasicBlock* succ, BasicBlock* pred) noexcept {
        assert(succ && "Link basic blocks got nullptr successor");
        assert(succ && "Link basic blocks got nullptr predcessor");
        //
        pred->add_succ(succ);
        succ->add_pred(pred);
    }

    /**
     * @brief Getters
     */
    id_type bb_id() const noexcept { return m_bb_id; }

    auto size() const noexcept { return m_instr.size(); }

    auto empty() const noexcept { return m_instr.empty(); }

    auto preds_num() const noexcept { return m_preds.size(); }
    auto succs_num() const noexcept { return m_succs.size(); }

    auto& preds() const noexcept { return m_preds; }
    auto& succs() const noexcept { return m_succs; }

    auto interval() const noexcept { return m_interval; }

    Function* parent() noexcept { return m_parent; }
    const Function* parent() const noexcept { return m_parent; }

    /**
     * @brief Setters
     */
    auto set_interval(LiveInterval other) noexcept { m_interval = other; }
    void set_parent(Function* parent) noexcept { m_parent = parent; }

    /// Get front/back instruction of the basic block
    auto& front() noexcept { return m_instr.front(); }
    const auto& front() const noexcept { return m_instr.front(); }

    auto& back() noexcept { return m_instr.back(); }
    const auto& back() const noexcept { return m_instr.back(); }

    /**
     * @brief Iterators based in ilist methods
     */
    iterator begin() { return m_instr.begin(); }
    const_iterator begin() const { return m_instr.begin(); }
    auto rbegin() { return std::reverse_iterator{end()}; }
    auto rbegin() const { return std::reverse_iterator{end()}; }

    iterator end() { return m_instr.end(); }
    const_iterator end() const { return m_instr.end(); }
    auto rend() { return std::reverse_iterator{begin()}; }
    auto rend() const { return std::reverse_iterator{begin()}; }

    ///
    void dump(std::ostream& os) {
        os << "bb id = " << m_bb_id << std::endl;
        for (auto&& instr : m_instr) instr.dump(os);
    }

private:
    template <typename T, class... Args>
    auto push_back(Args&&... args) {
        //
        static_assert(std::is_base_of<Instr, T>::value,
                      "Expected Instruction derived type");
        //
        auto* const inserted = static_cast<T*>(
            &emplace_back<T>(m_instr, std::forward<Args>(args)...));
        //
        // NOTE: here
        if constexpr (std::is_same_v<IfInstr, T>) {
            link_blocks(inserted->true_bb(), this);
            link_blocks(inserted->false_bb(), this);
        } else if constexpr (std::is_same_v<BranchInstr, T>)
            link_blocks(inserted->dst(), this);
        //

        // NOTE: is it really need ?
        inserted->set_parent(this);
        return inserted;
    }

    //
    friend IRBuilder;
};
}  // namespace jj_vm::ir
