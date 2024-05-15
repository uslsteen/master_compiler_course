#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
//
#include "pass_manager.hh"
#include "pass_visitor.hh"
//
#include "graph/dom3.hh"

namespace jj_vm::passes {

class ChecksElimination : Pass, PassVisitor {
public:
    using GraphTy = jj_vm::graph::BBGraph;
    using node_pointer = typename GraphTy::node_pointer;
    using node_iterator = typename GraphTy::node_iterator;

private:
    std::vector<std::reference_wrapper<jj_vm::ir::Instr>> m_instrs{};
    jj_vm::graph::dom3_impl::DomTree<GraphTy> m_tree{};

public:
    //
    void run(jj_vm::ir::Function* func) override {
        m_tree = jj_vm::graph::dom3_impl::DomTreeBuilder<GraphTy>::build(
            func->bb_graph());
        //
        visit_func(func);
        eliminate();
    }

    bool dominates(jj_vm::ir::Instr* dominator, jj_vm::ir::Instr* dominatee) {
        if (dominator == dominatee) return true;

        auto* dominator_bb = dominator->parent();
        auto* dominatee_bb = dominatee->parent();

        if (dominator_bb != dominatee_bb)
            return m_tree.dominates(dominator_bb, dominatee_bb);

        //! NOTE: could achive dominatee instruction, therefore lhs
        //! dominates rhs
        return std::find_if(ir::BasicBlock::iterator{dominator},
                            dominator_bb->end(),
                            [dominatee](jj_vm::ir::Instr& cur_instr) {
                                return (&cur_instr == dominatee);
                            }) != dominator_bb->end();
    }

    void visit_instr(jj_vm::ir::Instr& instr) override {
        switch (instr.opcode()) {
            case jj_vm::ir::Opcode::BOUNDS_CHECK:
            case jj_vm::ir::Opcode::NULL_CHECK: {
                m_instrs.push_back(instr);
                break;
            }
            default:
                break;
        }
    }

    void eliminate() {
        for (auto ref_instr : m_instrs) {
            auto& instr = ref_instr.get();
            switch (instr.opcode()) {
                case jj_vm::ir::Opcode::BOUNDS_CHECK: {
                    BoundsCheck(static_cast<jj_vm::ir::BinInstr&>(instr));
                    break;
                }
                case jj_vm::ir::Opcode::NULL_CHECK: {
                    NullCheck(static_cast<jj_vm::ir::UnaryInstr&>(instr));
                    break;
                }
                default:
                    assert(false && "Error: unsupported elimination check");
            }
        }
    }

    void NullCheck(jj_vm::ir::UnaryInstr& instr) {
        auto* input = instr.get_input(0);
        //
        for (auto user_it = input->users().begin();
             user_it != input->users().end(); ++user_it) {
            auto* user = *user_it;
            bool is_null_check =
                user->opcode() == jj_vm::ir::Opcode::NULL_CHECK;
            //
            if (is_null_check && (user != &instr) && dominates(user, &instr)) {
                input->remove_user(&instr);
                instr.parent()->erase(&instr);
                return;
            }
        }
    }

    void BoundsCheck(jj_vm::ir::BinInstr& instr) {
        auto* input = instr.get_input(0);
        auto* bounds = instr.get_input(1);
        //
        for (auto user_it = input->users().begin();
             user_it != input->users().end(); ++user_it) {
            auto* user = *user_it;
            bool is_bounds_check =
                user->opcode() == jj_vm::ir::Opcode::BOUNDS_CHECK;
            //
            if (is_bounds_check && (user != &instr)) {
                auto* user_bounds = user->get_input(1);
                if (bounds == user_bounds && dominates(user, &instr)) {
                    input->remove_user(&instr);
                    bounds->remove_user(&instr);
                    instr.parent()->erase(&instr);
                    return;
                }
            }
        }
    }
};
}  // namespace jj_vm::passes
