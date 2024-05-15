#pragma once

#include <unordered_set>
#include <vector>

#include "intrusive_list/ilist.hh"
#include "opcodes.hh"
//
#include <algorithm>
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
 *        I suppose this class may be usefull in future during CastInstr
 * implementation
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
    std::unordered_set<Instr*> m_users;

public:
    Value() = default;
    explicit Value(Type type) : m_type(type) {}

    auto& users() const { return m_users; }
    auto& users() { return m_users; }

    void remove_user(Instr* instr) {
        m_users.erase(instr);
    }
    void replace_users(Value& other);
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
protected:
    Opcode m_opcode = Opcode::NONE;
    BasicBlock* m_parent = nullptr;
    //
    std::size_t m_live{};
    std::size_t m_lin{};
    //
    std::vector<Value*> m_inputs;
    //
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

    auto live() const noexcept { return m_live; }
    auto lin() const noexcept { return m_lin; }

    auto inputs() const noexcept { return m_inputs; }
    Value* get_input(std::size_t id) const { return m_inputs.at(id); }

    auto begin() { return m_inputs.begin(); }
    auto begin() const { return m_inputs.begin(); }

    auto end() { return m_inputs.end(); }
    auto end() const { return m_inputs.end(); }

    /**
     * @brief Setters
     */
    void set_live(std::size_t live) { m_live = live; }
    void set_lin(std::size_t lin) { m_lin = lin; }

    void add_input(Value* val) {
        val->users().insert(this);
        m_inputs.push_back(val);
    }

    void set_input(std::size_t id, Value* val) {
        auto& inp = m_inputs[id];

        //! NOTE: check number of inputs
        if (std::count(m_inputs.begin(), m_inputs.end(), inp) == 1)
            inp->users().erase(this);
        //
        inp = val;
        inp->users().insert(this);
    }

    void clean_inputs() {
        for (auto* input : m_inputs) input->users().erase(this);

        m_inputs.clear();
    }

    virtual void dump(std::ostream& os) = 0;
    //
    friend IRBuilder;
    friend BasicBlock;
};

void Value::replace_users(Value& other) {
    auto& cur_users = users();
    cur_users.merge(other.users());
    other.users().clear();

    for (auto* user : cur_users) {
        std::replace(user->begin(), user->end(), &other,
                     this);
    }
}
}  // namespace jj_vm::ir
