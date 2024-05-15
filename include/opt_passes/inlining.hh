#pragma once

#include <sys/wait.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
//
#include "pass_manager.hh"
#include "pass_visitor.hh"

namespace jj_vm::passes {

class Inlining : Pass, PassVisitor {
public:
    using GraphTy = jj_vm::graph::BBGraph;
    using node_pointer = typename GraphTy::node_pointer;
    using node_iterator = typename GraphTy::node_iterator;

private:
    std::vector<std::reference_wrapper<jj_vm::ir::Instr>> m_instrs{};
    jj_vm::ir::IRBuilder m_builder{};
    //
    node_pointer m_callee_head{};

    static constexpr std::size_t kInlineInstrSize = 100;

public:
    //
    void run(jj_vm::ir::Function* func) override {
        visit_func(func);

        for (auto& ref_inst : m_instrs) {
            auto& call_instr =
                static_cast<jj_vm::ir::CallInstr&>(ref_inst.get());
            optimize(func, call_instr);
        }
    }

    void visit_instr(jj_vm::ir::Instr& instr) override {
        switch (instr.opcode()) {
            case jj_vm::ir::Opcode::CALL: {
                m_instrs.push_back(instr);
                break;
            }
            default:
                break;
        }
    }

    /**
     * @brief Function to update data flow for input parameters
                    1. move input parameters useer to callee input
     *
     * @param[in] callee
     * @param[in] instr
     */
    void update_parameters(jj_vm::ir::CallInstr& instr) {
        auto* callee = instr.callee();

        jj_vm::ir::BasicBlock* root = callee->bb_graph().head();
        auto input_params = root->collect(ir::Opcode::PARAM);
        auto input_size = input_params.size();
        auto inp_param_it = input_params.begin();

        assert(input_size == instr.inputs().size());
        //
        for (std::size_t i = 0; i < input_size; ++i) {
            auto* cur_input = instr.get_input(i);
            jj_vm::ir::Instr* cur_param = input_params.at(i);
            //
            cur_input->replace_users(*cur_param);
            jj_vm::ir::erase(root, cur_param);
        }
    }

    /**
     * @brief Function to update data flow for return
            1. collect all return instructions over the function
            2. there is only one return => move caller useers to return input
     instruction
            3. there are multiple return => collect all return ret
     values and pass it into PHI node, which would emplaced front into callee bb

     * @param[in] callee
     * @param[in] instr
     * @param[in] call_cont_bb
     */
    void update_return(jj_vm::ir::CallInstr& instr,
                       jj_vm::ir::BasicBlock* call_cont_bb) {
        auto* callee = instr.callee();

        //! NOTE: if there are no return values therefore nothing to update
        if (callee->func_ty().type() == jj_vm::ir::TypeId::NONE) return;
        //
        std::vector<jj_vm::ir::RetInstr*> callee_return;

        auto graph = callee->bb_graph();
        m_callee_head = graph.head();
        auto rpo = jj_vm::graph::deep_first_search_reverse_postoder(graph);

        //! NOTE: try move it into PassVisitor
        for (auto* bb : rpo) {
            auto& block = *bb;
            auto* last_instr = &block.back();
            if (last_instr->opcode() == jj_vm::ir::Opcode::RET)
                callee_return.push_back(
                    static_cast<jj_vm::ir::RetInstr*>(last_instr));
        }

        if (callee_return.size() == 1)
            callee_return.at(0)->retval()->replace_users(instr);
        else {
            m_builder.set_insert_point(call_cont_bb);
            auto* phi_node =
                m_builder.emplace_front<jj_vm::ir::PhiInstr>(callee->func_ty());
            //
            for (auto* ret : callee_return)
                phi_node->add_node(std::make_pair(
                    static_cast<jj_vm::ir::Instr*>(ret->retval()),
                    ret->parent()));
            //
            phi_node->replace_users(instr);
        }

        //! NOTE: replace all callee return to branch to splitted bb of caller
        for (auto* ret : callee_return) {
            auto* parent = ret->parent();
            jj_vm::ir::erase(parent, ret);
            //
            m_builder.set_insert_point(parent);
            m_builder.create<jj_vm::ir::BranchInstr>(call_cont_bb);
        }
    }

    void merge(jj_vm::ir::CallInstr& instr) {
        auto* callee = instr.callee();
        auto* parent = instr.parent();

        auto* callee_head = callee->bb_graph().head();
        //
        //! NOTE: add flow bettwen terminate bb of caller func & next
        //! after first bb of callee func
        parent->splice(parent->end(), *callee_head);
        callee->erase(callee_head);
    }

    void make_inline(jj_vm::ir::Function* caller, jj_vm::ir::CallInstr& instr,
                     jj_vm::ir::BasicBlock* call_cont_bb) {
        auto* callee = instr.callee();
        auto* parent = instr.parent();
        //
        jj_vm::ir::erase(parent, &instr);
        caller->splice(ir::Function::iterator{call_cont_bb}, *callee);
        //
        ir::BasicBlock::id_type new_id = 0;
        for (auto&& bb : *caller) {
            m_builder.set_bb_id(&bb, new_id);
            ++new_id;
        }
    }

    bool is_really_need_inlining(jj_vm::ir::Function* callee) {
        std::size_t callee_instr_size = 0;

        for (auto&& bb : *callee) callee_instr_size += bb.size();

        return callee_instr_size < kInlineInstrSize;
    }

    void optimize(jj_vm::ir::Function* caller, jj_vm::ir::CallInstr& instr) {
        auto* callee = instr.callee();

        //! NOTE: check instruction size of callee graph
        if (!is_really_need_inlining(callee)) return;

        auto* parent = instr.parent();

        //! NOTE: split block with a call instruction into call_block &
        //! call_cont_block
        auto* call_cont_bb = ir::split_bb_after(parent, &instr);
        //
        update_parameters(instr);
        update_return(instr, call_cont_bb);
        merge(instr);
        //
        make_inline(caller, instr, call_cont_bb);
    }
};
}  // namespace jj_vm::passes
