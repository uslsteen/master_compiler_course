#include "gtest/gtest.h"
//
#include "IR/basic_block.hh"
#include "IR/function.hh"
#include "IR/instruction.hh"
#include "IR/ir_builder.hh"
//

namespace jj_vm::ir::testing {

// Code:
// int64_t fact(int32_t n) {
//     int64_t res = 1;
//     for(int32_t i = 2; i <= n; ++i) {
//         res *= i;
//     }
//     return res;
// }

// Reference JJ IR Test:

TEST(fib, init) {
    //
    jj_vm::ir::IRBuilder builder{};
    //
    // def fact(v0 : i32) -> i64

    auto* fib_func = jj_vm::ir::Function::create_function<jj_vm::ir::Param>(
        jj_vm::ir::Type::create<jj_vm::ir::TypeId::I64>(), std::string{"fib"});

    auto* v0 = fib_func->create<jj_vm::ir::Param, jj_vm::ir::Type>(jj_vm::ir::TypeId::I32);
    //
    auto bb0 = fib_func->create<jj_vm::ir::BasicBlock>();
    auto bb1 = fib_func->create<jj_vm::ir::BasicBlock>();
    auto bb2 = fib_func->create<jj_vm::ir::BasicBlock>();
    auto bb3 = fib_func->create<jj_vm::ir::BasicBlock>();
    //
    {
        //! NOTE: fib function verification
        ASSERT_EQ(fib_func->name(), "fib");
        ASSERT_EQ(v0->type(), jj_vm::ir::TypeId::I32);
        //
        ASSERT_EQ(fib_func->front().bb_id(), 0);
        ASSERT_EQ(fib_func->back().bb_id(), 3);
        ASSERT_NE(&fib_func->front(), &fib_func->back());
    }

    jj_vm::ir::BasicBlock::link_blocks(bb1, bb0);
    jj_vm::ir::BasicBlock::link_blocks(bb2, bb1);
    jj_vm::ir::BasicBlock::link_blocks(bb3, bb1);
    //
    {
        //! NOTE: basic blocks instrusive list verification
        ASSERT_EQ(bb0->bb_id(), 0);
        ASSERT_EQ(bb1->bb_id(), 1);
        ASSERT_EQ(bb2->bb_id(), 2);
        ASSERT_EQ(bb3->bb_id(), 3);

        ASSERT_EQ(bb0->get_next(), bb1);
        ASSERT_EQ(bb1->get_next(), bb2);
        ASSERT_EQ(bb2->get_next(), bb3);
        //
        ASSERT_EQ(bb1->get_prev(), bb0);
        ASSERT_EQ(bb2->get_prev(), bb1);
        ASSERT_EQ(bb3->get_prev(), bb2);

        ASSERT_EQ(bb0->parent(), fib_func);
        ASSERT_EQ(bb1->parent(), fib_func);
        ASSERT_EQ(bb2->parent(), fib_func);
        ASSERT_EQ(bb3->parent(), fib_func);
    }
    //
    //
    //! NOTE: init bb0

    //   bb0:
    //     v1 = const i64 1 // res
    //     v2 = const i32 2 // i
    //     jmp bb1 // check_cond

    builder.set_insert_point(bb0);
    //
    auto* v1 = builder.create<jj_vm::ir::ConstI32>(1);
    auto* v2 = builder.create<jj_vm::ir::ConstI64>(2);
    //
    auto branch_bb0_bb1 = builder.create<jj_vm::ir::BranchInstr>(bb1);
    //
    {
        //! NOTE: verification bb0
        ASSERT_EQ(v1->type(), jj_vm::ir::TypeId::I32);
        ASSERT_EQ(v2->type(), jj_vm::ir::TypeId::I64);
        ASSERT_EQ(branch_bb0_bb1->opcode(), jj_vm::ir::Opcode::BRANCH);
        //
        ASSERT_EQ(v1->val(), 1);
        ASSERT_EQ(v2->val(), 2);
        //
        ASSERT_EQ(v1->parent(), bb0);
        ASSERT_EQ(v2->parent(), bb0);
        ASSERT_EQ(branch_bb0_bb1->parent(), bb0);
        //
        ASSERT_EQ(v1->get_next(), v2);
        ASSERT_EQ(v2->get_prev(), v1);
        ASSERT_EQ(branch_bb0_bb1->dst(), bb1);
        //
    }

    //! NOTE: init bb1

    //   bb1:
    //     v3 = phi i32 [v2, bb0], [v7, bb2]
    //     v4 = cmp le v3, v0
    //     v5 = phi i64 [v1, bb0], [v9, bb2]
    //     if v4, bb2, bb3

    builder.set_insert_point(bb1);

    //! NOTE: phi i32 def, not init
    auto* v3 = builder.create<jj_vm::ir::PhiInstr>(jj_vm::ir::TypeId::I32);

    //! NOTE: cmp le
    auto* v4 = builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::LE, v3, v0);
    //
    //! NOTE: phi i64 def, not init
    auto* v5 = builder.create<jj_vm::ir::PhiInstr>(jj_vm::ir::TypeId::I64);
    auto* if_v4 = builder.create<jj_vm::ir::IfInstr>(bb2, bb3, v4);
    //
    {
        //! NOTE: verification bb1
        ASSERT_EQ(v3->parent(), bb1);
        ASSERT_EQ(v4->parent(), bb1);
        ASSERT_EQ(v5->parent(), bb1);
        ASSERT_EQ(if_v4->parent(), bb1);
        //
        ASSERT_EQ(v3->get_next(), v4);
        ASSERT_EQ(v4->get_next(), v5);
        ASSERT_EQ(v5->get_next(), if_v4);
        //
        ASSERT_EQ(v4->get_prev(), v3);
        ASSERT_EQ(v5->get_prev(), v4);
        ASSERT_EQ(if_v4->get_prev(), v5);
        //
        ASSERT_EQ(v3->type(), jj_vm::ir::TypeId::I32);
        ASSERT_EQ(v5->type(), jj_vm::ir::TypeId::I64);
        //
        ASSERT_EQ(if_v4->true_bb(), bb2);
        ASSERT_EQ(if_v4->false_bb(), bb3);
        //
        ASSERT_NE(bb1->begin(), bb1->end());
    }

    //! NOTE: init bb2

    //   bb2:
    //     v6 = const i32 1
    //     v7 = add i32 v3, v6
    //     v8 = cast v3 to i64
    //     v9 = mul i64 v5, v8
    //     jmp bb1

    builder.set_insert_point(bb2);

    auto* v6 = builder.create<jj_vm::ir::ConstI32>(1);
    auto* v7 = builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD, v3, v6);
    auto* v8 = builder.create<jj_vm::ir::CastInstr>(jj_vm::ir::TypeId::I64, v3);
    auto* v9 = builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::MUL, v5, v8);
    //
    auto* branch_bb2_bb1 = builder.create<jj_vm::ir::BranchInstr>(bb1);
    //
    {
        //! NOTE: verification bb2
        ASSERT_EQ(v6->type(), jj_vm::ir::TypeId::I32);
        ASSERT_EQ(v6->val(), 1);
        //
        ASSERT_EQ(v6->get_next(), v7);
        ASSERT_EQ(v7->get_next(), v8);
        ASSERT_EQ(v8->get_next(), v9);
        ASSERT_EQ(v9->get_next(), branch_bb2_bb1);
        //
        ASSERT_EQ(v7->get_prev(), v6);
        ASSERT_EQ(v8->get_prev(), v7);
        ASSERT_EQ(v9->get_prev(), v8);
        ASSERT_EQ(branch_bb2_bb1->get_prev(), v9);
        //
        ASSERT_EQ(branch_bb2_bb1->dst(), bb1);
        ASSERT_EQ(v7->lhs(), v3);
        ASSERT_EQ(v7->rhs(), v6);
        ASSERT_EQ(v9->lhs(), v5);
        ASSERT_EQ(v9->rhs(), v8);
    }
    //! NOTE: init bb3

    //   bb3:
    //     ret v5

    builder.set_insert_point(bb3);

    builder.create<jj_vm::ir::RetInstr>(v5);

    //! NOTE: init phi nodse

    // v3 = phi i32 [v2, bb0], [v7, bb2]
    v3->add_node(std::make_pair(v2, bb0));
    v3->add_node(std::make_pair(v7, bb2));
    //
    // v5 = phi i64 [v1, bb0], [v9, bb2]
    v5->add_node(std::make_pair(v1, bb0));
    v5->add_node(std::make_pair(v7, bb2));
    
}

}