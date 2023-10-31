#ifndef INSTR_HH
#define INSTR_HH

#include "instruction.hh"
//
#include <ostream>
#include <vector>
#include <cassert>

// llvm::CastInst mm;

namespace jj_ir {

class IfInstr final : public Instr {
    //
    BasicBlock* m_true_bb = nullptr;
    BasicBlock* m_false_bb = nullptr;
    //
    Value* m_cond = nullptr;
    //
public:
    IfInstr(BasicBlock* true_bb, BasicBlock* false_bb, Value* cond = nullptr)
        : Instr(Opcode::IF),
          m_true_bb(true_bb),
          m_false_bb(false_bb),
          m_cond(cond) {}

    /**
     * @brief Getters
     */
    auto true_bb() const noexcept { return m_true_bb; }
    auto false_bb() const noexcept { return m_false_bb; }

    void dump(std::ostream& os) override {}
};

/**
 * @brief Branch insturction
 *
 */
class BranchInstr final : public Instr {
    BasicBlock* m_dst = nullptr;

public:
    BranchInstr(BasicBlock* dst) : Instr(Opcode::BRANCH), m_dst(dst) {}

    /**
     * @brief Getters
     */
    auto dst() const noexcept { return m_dst; }

    void dump(std::ostream& os) override {}
};

class RetInstr final : public Instr {
    Value* m_retval{};

public:
    RetInstr(Value* retval) : Instr(Opcode::RET), m_retval(retval) {}

    /**
     * @brief Getters
     */
    auto retval() const noexcept { return m_retval; }

    void dump(std::ostream& os) override {}
};

class BinOperator final : public Instr {
public:
    Value* m_lhs = nullptr;
    Value* m_rhs = nullptr;

protected:
    BinOperator(Opcode opc, Value* lhs, Value* rhs)
        : Instr(lhs->get_type(), opc), m_lhs(lhs), m_rhs(rhs) {
        //
        assert(lhs->get_type() == rhs->get_type());
    }

public:
    /**
     * @brief Getters
     */
    auto lhs() const noexcept { return m_lhs; }
    auto rhs() const noexcept { return m_rhs; }

    void dump(std::ostream& os) override {}
};

class PhiInstr final : public Instr {
    //
    std::vector<Instr*> m_instrs{};
    std::vector<BasicBlock*> m_basic_blocks{};
    //
public:
    PhiInstr(Type type) : Instr(type, Opcode::PHI) {}
    //
    void add_node(const std::pair<Instr*, BasicBlock*> input) {
        m_instrs.push_back(input.first);
        m_basic_blocks.push_back(input.second);
    }

    void dump(std::ostream& os) override {}
};

//! NOTE: maybe inherit public UnaryInstr in future ???
class CastInstr final : public Instr {
    Value* m_src_val = nullptr;

public:
    CastInstr(Type ty, Value* val) : Instr(ty, Opcode::CAST), m_src_val(val) {}

    /**
     * @brief Getters
     */
    auto src_val() const noexcept { return m_src_val; }

    void dump(std::ostream& os) override {}
};

class Constant final : public Instr {
    //
};

}  // namespace jj_ir

#endif  // INSTR_HH