#ifndef INSTR_HH
#define INSTR_HH

#include "intrusive_list/ilist.hh"
#include "opcodes.hh"
//
#include <array>

namespace jj_ir {

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
    TypeId get_type() const noexcept { return m_type.get_type(); }
};

/**
 * @brief Linear part of the code
 *
 */
class BasicBlock final : public Value {// , public INode<BasicBlock> {
public:
    //! NOTE: Instruction iterators
    // using iterator = InstListType::iterator;
    // using const_iterator = InstListType::const_iterator;
    // using reverse_iterator = InstListType::reverse_iterator;
    // using const_reverse_iterator = InstListType::const_reverse_iterator;

    static constexpr int MaxSucNum = 2;

private:
    std::uint32_t m_bb_id = 0;
    //IntrusiveList<Instr> m_instrs;
    //
    Instr* m_first_instr = nullptr;
    Instr* m_last_instr = nullptr;
    //
    std::array<BasicBlock*, MaxSucNum> m_success;
    // std::vector<BasicBlock*> m_predecess;
    //
    friend IRBuilder;

public:
    BasicBlock() = default;
    explicit BasicBlock(std::uint32_t bb_id) : m_bb_id(bb_id) {}

    auto bb_id() const noexcept { return m_bb_id; }

    //auto size() const noexcept { return m_instrs.size(); }
    //pointer first_instr() const noexcept { return m_first_instr; }
    //pointer last_instr() const noexcept { return m_last_instr; }
};

class Instr : public Value {//, public INode<Instr> {
    //
    BasicBlock* m_parent = nullptr;

protected:
    Opcode m_opcode = Opcode::NONE;
    //
public:
    Instr() = default;
    Instr(Type type) : Value{type} {}
    Instr(Type type, BasicBlock* bb) : Value{type}, m_parent(bb) {}
    //
    virtual ~Instr() = default;
    //
    auto opcode() const noexcept { return m_opcode; }
    auto type() const noexcept { return m_type; }
    //
    const BasicBlock* parent() const { return m_parent; }
    BasicBlock* parent() { return m_parent; }
    //    
    friend IRBuilder;
};

class IfInstr final : public Instr {
    //
    BasicBlock* m_true = nullptr;
    BasicBlock* m_false = nullptr;
    //
};

class BinInstr final : public Instr {
private:
    // std::array<Value*, 2> m_inputs{};
};

class PhiInstr final : public Instr {
    //
};

class CastInstr final : public Instr {
    //
};

class Constant final : public Instr {
    //
};

class CmpInstr final : public Instr {
    //
};

class IRBuilder final {
    //
private:
    BasicBlock* m_bb;

public:
    /**
     * @brief This specifies that created instructions should be appended to the
     *        end of the specified block.
     * @param[in] TheBB
     */
    void SetInsertPoint(BasicBlock* TheBB) {
        // BB = TheBB;
        // InsertPt = BB->end();
    }

    /**
     * @brief This specifies that created instructions should be inserted before
     *        the specified instruction.
     * @param[in] I
     */
    void SetInsertPoint(Instr* I) {
        // BB = I->getParent();
        // InsertPt = I->getIterator();
        // assert(InsertPt != BB->end() && "Can't read debug loc from end()");
        // SetCurrentDebugLocation(I->getDebugLoc());
    }
};

}  // namespace jj_ir

#endif  // INSTR_HH