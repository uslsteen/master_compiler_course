#ifndef INSTR_HH
#define INSTR_HH

#include "intrusive_list/ilist.hh"
#include "opcodes.hh"
//
#include <array>
#include <ostream>
#include <vector>

// llvm::CastInst mm;

namespace jj_ir {

using namespace ilist_detail;

/**
 * @brief Enum to describe instruction type
 *
 */
enum class TypeId : uint8_t {
    NONE = 0,
    I8,
    I16,
    I32,
    I64,
};

class IRBuilder;
class Instr;
class BasicBlock;

class Type final {
    TypeId m_id;

public:
    Type(TypeId id = TypeId::NONE) : m_id(id) {}
    TypeId get_type() const noexcept { return m_id; }
};

class Value {
protected:
    Type m_type = TypeId::NONE;

public:
    Value() = default;
    Value(Type type) : m_type(type) {}
    //
    /**
     * @brief Getters
     */
    TypeId get_type() const noexcept { return m_type.get_type(); }
};

/**
 * @brief Base Instruction class
          Instruction inherits Value due provide parameterized type of some
          instructions
 *
 */
class Instr : public Value, public ilist_node {
    //
    Opcode m_opcode = Opcode::NONE;
    BasicBlock* m_parent = nullptr;
    //
protected:
    Instr() = default;
    //
    Instr(Opcode opc, BasicBlock* bb = nullptr) : m_opcode(opc), m_parent(bb) {}
    Instr(Type type, Opcode opc = Opcode::NONE, BasicBlock* bb = nullptr)
        : Value{type}, m_opcode(opc), m_parent(bb) {}

    void set_parent(BasicBlock* parent) noexcept { m_parent = parent; }
    //
public:
    Instr(const Instr&) = delete;
    Instr& operator=(const Instr&) = delete;
    //
    virtual ~Instr() = default;

    /**
     * @brief Getters
     */
    auto opcode() const noexcept { return m_opcode; }
    auto type() const noexcept { return m_type; }
    //
    const BasicBlock* parent() const { return m_parent; }
    BasicBlock* parent() { return m_parent; }

    virtual void dump(std::ostream& os) = 0;
    //
    //
    friend IRBuilder;
};

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