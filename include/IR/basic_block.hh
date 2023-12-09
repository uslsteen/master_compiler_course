#pragma once

#include <cassert>
//
#include <type_traits>
#include <utility>
//
#include "instruction.hh"
#include "instructions.hh"
#include "intrusive_list/ilist.hh"

namespace jj_ir {

class Function;

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
    using reverse_iterator = InstrList::iterator;
    using const_reverse_iterator = InstrList::const_reverse_iterator;
    //
private:
    uint32_t m_bb_id = 0;
    InstrList m_instr{};
    //
    std::vector<BasicBlock*> m_preds{};
    std::vector<BasicBlock*> m_succs{};
    //
    Function* m_parent = nullptr;
    //
    void set_parent(Function* parent) noexcept { m_parent = parent; }

    //! NOTE: it should be hidden in private due access to private data & dumb
    //!       func naming
    void add_succ(BasicBlock* succ) noexcept { m_succs.push_back(succ); }
    void add_pred(BasicBlock* pred) noexcept { m_preds.push_back(pred); }

    //
public:
    BasicBlock() = default;
    explicit BasicBlock(std::uint32_t bb_id, Function* parent = nullptr)
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
    auto bb_id() const noexcept { return m_bb_id; }

    auto size() const noexcept { return m_instr.size(); }

    auto empty() const noexcept { return m_instr.empty(); }

    auto preds_num() const noexcept { return m_preds.size(); }
    auto succs_num() const noexcept { return m_succs.size(); }

    auto& preds() const noexcept { return m_preds; }
    auto& succs() const noexcept { return m_succs; }

    Function* parent() noexcept { return m_parent; }
    const Function* parent() const noexcept { return m_parent; }

    /// Get front/back instruction of the basic block
    auto& front() const noexcept { return m_instr.front(); }
    auto& back() const noexcept { return m_instr.back(); }

    /**
     * @brief Iterators based in ilist methods
     */
    iterator begin() { return m_instr.begin(); }
    const_iterator begin() const { return m_instr.begin(); }

    iterator end() { return m_instr.end(); }
    const_iterator end() const { return m_instr.end(); }

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
}  // namespace jj_ir
