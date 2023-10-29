#pragma once

#include <type_traits>
#include <utility>

#include "intrusive_list/ilist.hh"
#include "ir.hh"

namespace jj_ir {

/**
 * @brief Linear part of the code
 *
 */
class BasicBlock final : public Value,  // maybe it will be useful in future
                                        //
                         public ilist_node {
public:
    using InstrList = ilist<Instr>;
    //
    using iterator = InstrList::iterator;
    using const_iterator = InstrList::const_iterator;
    using reverse_iterator = InstrList::iterator;
    using const_reverse_iterator = InstrList::const_reverse_iterator;

private:
    uint32_t m_bb_id = 0;
    InstrList m_instr{};
    //
    std::vector<BasicBlock*> m_preds{};
    std::vector<BasicBlock*> m_succs{};
    //

public:
    BasicBlock() = default;
    explicit BasicBlock(std::uint32_t bb_id) : m_bb_id(bb_id) {}

    /**
     * @brief Getters
     */
    auto bb_id() const noexcept { return m_bb_id; }

    auto size() const noexcept { return m_instr.size(); }
    auto empty() const noexcept { return m_instr.empty(); }

    auto preds_num() const noexcept { return m_preds.size(); }
    auto succs_num() const noexcept { return m_succs.size(); }

    /// Get front/back instruction of the basic block
    auto& front() const noexcept { return m_instr.front(); }
    auto& back() const noexcept { return m_instr.back(); }

    /**
     * @brief Iterators based in ilist methods
     */
    iterator begin() { return m_instr.begin(); }
    const_iterator begin() const { return m_instr.begin(); }

    iterator end() { return m_instr.end(); }
    const_iterator end() const { return m_instr.begin(); }


    ///
    void dump(std::ostream& os) {
        os << m_bb_id << std::endl;
        for (auto&& instr : m_instr) instr.dump(os);
    }

    //! NOTE: maybe it should private interface
    //! NOTE: therefore only IRBuilder can modify and insert insturctions (more safety ???)
    template <typename T, class... Args>
    auto emplace_back(Args&&... args) {
        static_assert(std::is_base_of<T, Instr>::value, "");
        //
        auto* const insterted = static_cast<T*>(
            &emplace_back<T>(m_instr, std::forward<Args>(args)...));
        //
        return insterted;
    }
    friend IRBuilder;
};
}  // namespace jj_ir