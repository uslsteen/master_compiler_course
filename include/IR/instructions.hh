#ifndef INSTR_HH
#define INSTR_HH

#include "instruction.hh"
//
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <vector>

namespace jj_vm::ir {

class IfInstr final : public Instr {

    BasicBlock* m_true_bb = nullptr;
    BasicBlock* m_false_bb = nullptr;
    //
    Value* m_cond = nullptr;

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

    /// Override dump
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

    /// Override dump
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

    /// Override dump
    void dump(std::ostream& os) override {}
};

class BinInstr final : public Instr {
public:
    Value* m_lhs = nullptr;
    Value* m_rhs = nullptr;

    // protected:
public:
    BinInstr(Opcode opc, Value* lhs, Value* rhs)
        : Instr(lhs->type(), opc), m_lhs(lhs), m_rhs(rhs) {
        //
        assert(lhs->type() == rhs->type());
    }

public:
    /**
     * @brief Getters
     */
    auto lhs() const noexcept { return m_lhs; }
    auto rhs() const noexcept { return m_rhs; }

    /// Override dump
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

    /// Override dump
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

    /// Override dump
    void dump(std::ostream& os) override {}
};

template <typename Type>
class Constant : public Instr {
    //
    static_assert(std::numeric_limits<Type>::is_integer,
                  "Standart integral type required.");
};

/**
 * @brief Mapping standart c++ types to jj_vm custom types defined in TypeId
 *        enum
 */
#define CONSTANT_SPEC(cstd_ty, jj_ir_ty)                            \
    template <>                                                     \
    class Constant<cstd_ty> : public Instr {                        \
    private:                                                        \
        cstd_ty m_val;                                              \
                                                                    \
    public:                                                         \
        Constant(cstd_ty val)                                       \
            : Instr{TypeId::jj_ir_ty, Opcode::CONST}, m_val(val) {} \
                                                                    \
        void dump(std::ostream& stream) override {}                 \
                                                                    \
        cstd_ty val() const { return m_val; }                       \
    };                                                              \
                                                                    \
    using Const##jj_ir_ty = Constant<cstd_ty>;

//

CONSTANT_SPEC(bool, I1);
CONSTANT_SPEC(int8_t, I8);
CONSTANT_SPEC(int16_t, I16);
CONSTANT_SPEC(int32_t, I32);
CONSTANT_SPEC(int64_t, I64);

}  // namespace jj_vm::ir

#endif  // INSTR_HH