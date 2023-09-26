#ifndef INSTR_HH
#define INSTR_HH

#include "instrusive_list.hh"
#include "opcodes.hh"

namespace jj_ir {

enum class Type : uint8_t {
    NONE,
    I8,
    I16,
    I32,
    I64,
};

class IRBuilder;
class Instr;

class BasicBlock final {
public:
    using value_type = IntrusiveList<Instr>::value_type;

private:
    std::uint32_t m_bb_id = 0;
    IntrusiveList<Instr> m_instrs;
    //
    Instr* m_first_instr = nullptr;
    Instr* m_last_instr = nullptr;

    //
    friend IRBuilder;

public:
    explicit BasicBlock(std::uint32_t bb_id) : m_bb_id(bb_id) {}

    auto bb_id() const noexcept { return m_bb_id; }

    auto size() const noexcept { return m_instrs.size(); }

    auto first_instr() const noexcept { return m_first_instr; }
    auto last_instr() const noexcept { return m_last_instr; }
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
public:
    Instr() = default;
    Instr(Type type) : m_type(type) {}

    virtual ~Instr() = default;
    //
    auto id() const noexcept { return m_id; }
    auto bb_id() const noexcept { return m_bb_id; }
    auto opcode() const noexcept { return m_opcode; }
    auto type() const noexcept { return m_type; }
};

class IRBuilder final {
    //
};

}  // namespace jj_ir

#endif  // INSTR_HH