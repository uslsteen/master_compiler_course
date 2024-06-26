#pragma once

#include "basic_block.hh"
#include "instructions.hh"

namespace jj_vm::ir {

class IRBuilder final {
    //
public:
    using iterator = BasicBlock::iterator;

private:
    BasicBlock* m_bb = nullptr;
    iterator m_insert_pt;

public:
    IRBuilder() = default;
    IRBuilder(BasicBlock* bb) : m_bb(bb), m_insert_pt(bb->m_instr.end()) {}

    /**
     * @brief Interface to create basic block that contains any Instruction
              After allocation parent basic block should be set

     * @param[in] args
     * @return T*
     */
    template <typename T, typename... Args>
    T* create(Args&&... args) {
        auto* const created_instr =
            m_bb->push_back<T>(std::forward<Args>(args)...);
        //
        created_instr->set_parent(m_bb);
        return created_instr;
    }

    template <typename T, typename... Args>
    T* emplace_front(Args&&... args) {
        auto* const created_instr =
            m_bb->push_front<T>(std::forward<Args>(args)...);
        //
        created_instr->set_parent(m_bb);
        return created_instr;
    }

    /**
     * @brief This specifies that created instructions should be appended to the
     *        end of the specified block.
     * @param[in] TheBB
     */
    void set_insert_point(BasicBlock* bb) {
        m_bb = bb;
        m_insert_pt = m_bb->m_instr.end();
    }

    void set_bb_id(BasicBlock* bb, BasicBlock::id_type id) { bb->set_id(id); }

    /**
     * @brief This specifies that created instructions should be inserted before
     *        the specified instruction.
     * @param[in] I
     */
    void set_insert_point(Instr* instr) {
        m_bb = instr->parent();
        m_insert_pt = iterator{instr};
    }
};

}  // namespace jj_vm::ir
