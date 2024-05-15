#pragma once

#include <string_view>
#include <type_traits>

#include "basic_block.hh"
#include "graph/bb_graph.hh"
#include "instruction.hh"

namespace jj_vm::ir {

using namespace ilist_detail;

class Param : public Value, public ilist_node {
public:
    Param(Type type) : Value(type) {}
};

/**
 * @brief
 *
 */
class Function final {
public:
    using BasicBlockList = ilist<BasicBlock>;
    using ParamList = ilist<Param>;
    //
    using iterator = BasicBlockList::iterator;
    using const_iterator = BasicBlockList::const_iterator;

private:
    BasicBlockList m_basic_blocks;
    ParamList m_args;
    //
    Type m_func_ty{};
    std::string m_func_name{};
    //
public:
    Function() = default;
    Function(Type func_ty, const std::string& func_name)
        : m_func_ty(func_ty), m_func_name(func_name) {}

    template <typename T, typename... Args>
    static Function* create_function(Type&& func_ty,
                                     const std::string& func_name,
                                     Args&&... args) {
        auto* cur_func = new Function{func_ty, func_name};
        //
        if constexpr (sizeof...(Args) > 0) {
            if constexpr (std::is_same_v<T, BasicBlock>)
                emplace_back<T>(cur_func->m_basic_blocks,
                                std::forward<Args>(args)...);
            //
            else if constexpr (std::is_same_v<T, Param>)
                emplace_back<T>(cur_func->m_args, std::forward<Args>(args)...);
            //
        }
        //
        return cur_func;
    }

    template <typename T, typename... Args>
    T* create(Args&&... args);

    /**
     * @brief BasicBlock iterator forwarding functions
     * @return iterator
     */
    iterator begin() { return m_basic_blocks.begin(); }
    const_iterator begin() const { return m_basic_blocks.begin(); }
    //
    iterator end() { return m_basic_blocks.end(); }
    const_iterator end() const { return m_basic_blocks.end(); }

    /// O(n)
    size_t size() const { return m_basic_blocks.size(); }

    /// O(1)
    bool empty() const { return m_basic_blocks.empty(); }
    //

    /**
     * @brief Accessors methods to basic block into function
     * @return const/non const BasicBlock&
     */
    const BasicBlock& front() const { return m_basic_blocks.front(); }
    BasicBlock& front() { return m_basic_blocks.front(); }

    const BasicBlock& back() const { return m_basic_blocks.back(); }
    BasicBlock& back() { return m_basic_blocks.back(); }
    //

    std::string_view name() const noexcept { return m_func_name; }
    Type func_ty() const noexcept { return m_func_ty; }

    auto bb_graph() noexcept {
        assert(!m_basic_blocks.empty() &&
               "Error : function hasn't any basic blocks to create graph");
        return jj_vm::graph::BBGraph{&m_basic_blocks.front(),
                                     m_basic_blocks.size()};
    }

    void erase(BasicBlock* to_erase) {
        m_basic_blocks.erase(iterator{to_erase});
    }

    void splice(iterator pos, Function& src) {
        m_basic_blocks.splice(pos, src.m_basic_blocks);
    }
};

template <typename T, typename... Args>
T* Function::create(Args&&... args) {}

/**
 * @brief Append function
 */

template <>
BasicBlock* Function::create<BasicBlock>() {
    uint32_t bb_id =
        m_basic_blocks.empty() ? 0 : m_basic_blocks.back().bb_id() + 1;
    //
    return &emplace_back<BasicBlock>(m_basic_blocks, bb_id, this);
}

template <>
Param* Function::create<Param>(Type&& type) {
    return &emplace_back<Param>(m_args, type);
}

BasicBlock* split_bb_after(BasicBlock* block, Instr* instr) {
    assert(instr->parent() == block);
    //
    Function* parent = block->parent();
    BasicBlock* new_block = block->parent()->create<jj_vm::ir::BasicBlock>();
    new_block->splice(new_block->end(), std::next(BasicBlock::iterator{instr}),
                      instr->parent()->end());
    //
    return new_block;
}

class CallInstr final : public Instr {
    jj_vm::ir::Function* m_callee{};

public:
    explicit CallInstr(Type type, jj_vm::ir::Function* callee)
        : Instr(type, jj_vm::ir::Opcode::CALL), m_callee(callee) {}

    void add_arg(Value* arg) { add_input(arg); }

    jj_vm::ir::Function* callee() const { return m_callee; }

    /// Override dump
    void dump(std::ostream& os) override {}
};
}  // namespace jj_vm::ir
