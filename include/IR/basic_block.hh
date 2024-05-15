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
#include "opcodes.hh"

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
        : m_begin(begin), m_end(end) {}

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

    static void remove_link(BasicBlock* succ, BasicBlock* pred) noexcept {
        auto& preds = succ->m_preds;
        preds.erase(std::find(preds.begin(), preds.end(), pred));
        //
        auto& succs = pred->m_succs;
        succs.erase(std::find(succs.begin(), succs.end(), succ));
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

    /**
     * @brief Modifiers
     */
    auto erase(Instr* instr) { return m_instr.erase(iterator{instr}); }

    void replace_instr(Instr* old_instr, Instr* new_instr) {
        assert(old_instr->parent() == this);
        new_instr->replace_users(*old_instr);
        m_instr.insert(erase(old_instr), new_instr);
        new_instr->set_parent(this);
    }

    void splice(iterator pos, BasicBlock& other) {
        splice(pos, other.begin(), other.end());
    }

    void splice(iterator pos, iterator first, iterator last) {
        std::for_each(first, last, [this](jj_vm::ir::Instr& instr) {
            instr.set_parent(this);
        });
        bool is_end = (pos == m_instr.end());

        m_instr.splice(pos, first, last);

        if (is_end) update();
    }

    void update() {
        for (auto succ : m_succs) remove_link(this, succ);
        if (m_instr.empty())
            return;
        else {
            auto& last_instr = m_instr.back();
            if (last_instr.opcode() == Opcode::BRANCH) {
                const auto& branch_instr =
                    static_cast<const BranchInstr&>(last_instr);
                link_blocks(branch_instr.dst(), this);
            } else if (last_instr.opcode() == Opcode::IF) {
                const auto& if_instr = static_cast<const IfInstr&>(last_instr);
                link_blocks(if_instr.true_bb(), this);
                link_blocks(if_instr.false_bb(), this);
            }
        }
    }

    std::vector<Instr*> collect(Opcode opc) const noexcept {
        std::vector<Instr*> collected{};

        for (auto it = begin(); it != end(); ++it) {
            const auto* pinstr = &*it;
            if (pinstr->opcode() == opc)
                collected.push_back(const_cast<Instr*>(pinstr));
        }
        return collected;
    }

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
    void set_parent(Function* parent) noexcept { m_parent = parent; }
    void set_id(id_type id) { m_bb_id = id; }

    template <typename T, class... Args>
    auto push_back(Args&&... args) {
        //
        static_assert(std::is_base_of<Instr, T>::value,
                      "Error: expected Instruction derived type");
        //
        auto* const inserted = static_cast<T*>(
            &emplace_back<T>(m_instr, std::forward<Args>(args)...));
        //
        if constexpr (std::is_same_v<IfInstr, T>) {
            link_blocks(inserted->true_bb(), this);
            link_blocks(inserted->false_bb(), this);
        } else if constexpr (std::is_same_v<BranchInstr, T>)
            link_blocks(inserted->dst(), this);
        //
        inserted->set_parent(this);
        return inserted;
    }

    template <typename T, class... Args>
    auto push_front(Args&&... args) {
        //
        static_assert(std::is_base_of<Instr, T>::value,
                      "Error:expected Instruction derived type");
        //
        auto* const inserted = static_cast<T*>(
            &emplace_front<T>(m_instr, std::forward<Args>(args)...));
        //
        // if constexpr (std::is_same_v<IfInstr, T>) {
        //     link_blocks(inserted->true_bb(), this);
        //     link_blocks(inserted->false_bb(), this);
        // } else if constexpr (std::is_same_v<BranchInstr, T>)
        //     link_blocks(inserted->dst(), this);
        //
        inserted->set_parent(this);
        return inserted;
    }

    //
    friend IRBuilder;
};

void erase(BasicBlock* bb, Instr* instr) {
    assert(bb == instr->parent());
    instr->clean_inputs();
    bb->erase(instr);
}

void erase(Instr* instr) {
    instr->clean_inputs();
    instr->parent()->erase(instr);
}
}  // namespace jj_vm::ir
