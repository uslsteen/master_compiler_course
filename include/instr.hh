#ifndef INSTR_HH
#define INSTR_HH

#include "instrusive_list.hh"
#include "ir.hh"
#include "opcodes.hh"

namespace jj_ir {

enum class Type : uint8_t {
    NONE,
    I8,
    I16,
    I32,
    I64,
};

class Instr : public INode<Instr> {
    //
    uint32_t m_id = 0;
    uint32_t m_bb_id = 0;
    Opcode m_opcode = Opcode::NONE;
    Type m_type = Type::NONE;
    //
    friend IRBuilder;
    //
protected:
    Instr() = default;
    Instr(Type type) : m_type(type) {}

public:
    auto id() const noexcept { return m_id; }
    auto bb_id() const noexcept { return m_bb_id; }
    auto opcode() const noexcept { return m_opcode; }
    auto type() const noexcept { return m_type; }
};

}  // namespace jj_ir

#endif  // INSTR_HH