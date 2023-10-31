#pragma once

#include <type_traits>

#include "basic_block.hh"
#include "instruction.hh"

namespace jj_ir {

using namespace ilist_detail;

struct Param : public Value, public ilist_node {
    explicit Param(Type type) : Value(type) {}
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
    Type m_func_ty = TypeId::NONE;
    std::string m_func_name{};

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
        if constexpr (std::is_same_v<T, BasicBlock>)
            emplace_back<T>(cur_func->m_basic_blocks,
                            std::forward<Args>(args)...);
        //
        else if constexpr (std::is_same_v<T, Param>)
            emplace_back<T>(cur_func->m_args, std::forward<Args>(args)...);

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
    iterator end() { return m_basic_blocks.end(); }
    const_iterator end() const { return m_basic_blocks.end(); }

    size_t size() const { return m_basic_blocks.size(); }
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
    return &emplace_back<BasicBlock>(m_basic_blocks, bb_id);
}

template <>
Param* Function::create<Param, Type>(Type&& type) {
    return &emplace_back<Param>(m_args, type);
}

}  // namespace jj_ir