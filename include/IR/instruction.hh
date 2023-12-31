#pragma once

#include "intrusive_list/ilist.hh"
#include "opcodes.hh"
//
#include <iostream>

namespace jj_vm::ir {

/**
 * @brief Enum to describe instruction type
 *
 */
enum class TypeId : uint8_t {
    NONE = 0,
    I1 = 1,
    I8,
    I16,
    I32,
    I64,
};

class IRBuilder;
class Instr;
class BasicBlock;


/**
 * @brief Type enum wrapper for standart jj_vm types, which described in Typeid
 *        I suppose this class may be usefull in future during CastInstr implementation
 */
class Type final {
    TypeId m_id;

public:
    //! FIXME: maybe it unsafe make it non explicit, but currently
    ///        I wanna have opportunity to create Type{} implicitly using TypeId
    Type(TypeId id = TypeId::NONE) : m_id(id) {}
    TypeId type() const noexcept { return m_id; }

    ///
    template <TypeId id>
    static Type create() {
        return Type{id};
    }
};

/***/
class Value {
protected:
    Type m_type{};

public:
    Value() = default;
    explicit Value(Type type) : m_type(type) {}
    //
    /**
     * @brief Getters
     */
    TypeId type() const noexcept { return m_type.type(); }
};

/**
 * @brief Base Instruction class
          Instruction inherits Value due provide parameterized type of some
          instructions
 *
 */
class Instr : public Value, public ilist_detail::ilist_node {
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
    // auto type() const noexcept { return m_type; }
    //
    const BasicBlock* parent() const { return m_parent; }
    BasicBlock* parent() { return m_parent; }

    virtual void dump(std::ostream& os) = 0;
    //
    //
    friend IRBuilder;
    friend BasicBlock; 
};
}  // namespace jj_vm::ir