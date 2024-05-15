#ifndef INSTR_HH
#define INSTR_HH

#include "basic_block.hh"
#include "instruction.hh"
#include "opcodes.hh"
//
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <vector>

namespace jj_vm::ir {

class IfInstr final : public Instr {
    //
    BasicBlock* m_true_bb = nullptr;
    BasicBlock* m_false_bb = nullptr;
    //
    Value* m_cond = nullptr;

public:
    IfInstr(BasicBlock* true_bb, BasicBlock* false_bb, Value* cond = nullptr)
        : Instr(Opcode::IF),
          m_true_bb(true_bb),
          m_false_bb(false_bb),
          m_cond(cond) {
        add_input(cond);
    }

    /**
     * @brief Getters
     */
    auto true_bb() const noexcept { return m_true_bb; }
    auto false_bb() const noexcept { return m_false_bb; }
    auto cond() const noexcept { return get_input(0); }

    /// Override dump
    void dump(std::ostream& os) override {}
};

/**
 * @brief Branch insturction
 *
 */
class BranchInstr final : public Instr {
    //
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
    RetInstr(Value* retval) : Instr(Opcode::RET), m_retval(retval) {
        add_input(retval);
    }

    /**
     * @brief Getters
     */
    auto retval() const noexcept { return get_input(0); }

    /// Override dump
    void dump(std::ostream& os) override {}
};

class BinInstr final : public Instr {
    //
    Value* m_lhs = nullptr;
    Value* m_rhs = nullptr;

public:
    BinInstr(Opcode opc, Value* lhs, Value* rhs)
        : Instr(lhs->type(), opc), m_lhs(lhs), m_rhs(rhs) {
        //
        assert(lhs->type() == rhs->type());
        add_input(lhs);
        add_input(rhs);
    }

public:
    /**
     * @brief Getters
     */
    auto lhs() const noexcept { return get_input(0); }
    auto rhs() const noexcept { return get_input(1); }

    /// Override dump
    void dump(std::ostream& os) override {}
};

class UnaryInstr final : public Instr {
public:
    UnaryInstr(Opcode opc, Value* val) : Instr(val->type(), opc) {
        add_input(val);
    }

    auto val() const noexcept { return get_input(0); }

    /// Override dump
    void dump(std::ostream& os) override {}
};

class PhiInstr final : public Instr {
    //
public:
    using phi_var_pair = std::pair<Instr*, BasicBlock*>;

private:
    std::vector<std::pair<Instr*, BasicBlock*>> m_vars;

public:
    PhiInstr(Type type) : Instr(type, Opcode::PHI) {}
    //
    void add_node(const std::pair<Instr*, BasicBlock*> input) {
        m_vars.push_back(input);
        add_input(input.first);
    }

    /**
     * @brief Getters
     */
    auto vars() const noexcept { return m_vars; }

    /// Override dump
    void dump(std::ostream& os) override {}
};

//! NOTE: maybe inherit public UnaryInstr in future ???
class CastInstr final : public Instr {
    //
    Value* m_src_val = nullptr;

public:
    CastInstr(Type ty, Value* val) : Instr(ty, Opcode::CAST), m_src_val(val) {
        add_input(val);
    }

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

/**
 * @brief
 */
class ParamInstr final : public Instr {
    std::string m_name{};

public:
    ParamInstr(std::string&& name, Type type)
        : Instr(type, jj_vm::ir::Opcode::PARAM), m_name(std::move(name)) {}

    auto name() const noexcept { return m_name; }

    /// Override dump
    void dump(std::ostream& os) override {}
};
}  // namespace jj_vm::ir

#endif  // INSTR_HH
