#include <cstdint>
#include "gtest/gtest.h"
//
#include "IR/basic_block.hh"
#include "IR/function.hh"
#include "IR/instruction.hh"
#include "IR/ir_builder.hh"
//

// Code:
// int64_t fact(int32_t n) {
//     int64_t res = 1;
//     for(int32_t i = 2; i <= n; ++i) {
//         res *= i;
//     }
//     return res;
// }

// IR:
// def fact(v0 : i32) -> i64 {
//   bb0:
//     v1 = const i64 1 // res
//     v2 = const i32 2 // i
//     jmp bb1 // check_cond

//   bb1:
//     v3 = phi i32 [v2, bb0], [v7, bb2]
//     v4 = cmp le v3, v0
//     v5 = phi i64 [v1, bb0], [v9, bb2]
//     if v4, bb2, bb3

//   bb2:
//     v6 = const i32 1
//     v7 = add i32 v3, v6
//     v8 = cast v3 to i64
//     v9 = mul i64 v5, v8
//     jmp bb1
//

//   bb3:
//     ret v5
// }

TEST(Function, init) {
    // jj_ir::BasicBlock bb{1};
    // jj_ir::IRBuilder builder{&bb};

    auto* fib_func = jj_ir::Function::create_function<jj_ir::Param>(
        jj_ir::Type::create<jj_ir::TypeId::I64>(), std::string{"fib"},
        jj_ir::Type::create<jj_ir::TypeId::I32>());
    //

    auto bb0 = fib_func->create<jj_ir::BasicBlock>();
    auto bb1 = fib_func->create<jj_ir::BasicBlock>();
    auto bb2 = fib_func->create<jj_ir::BasicBlock>();
    auto bb3 = fib_func->create<jj_ir::BasicBlock>();
    //
    {
        ASSERT_EQ(bb0->bb_id(), 0);
        ASSERT_EQ(bb1->bb_id(), 1);
        ASSERT_EQ(bb2->bb_id(), 2);
        ASSERT_EQ(bb3->bb_id(), 3);
    }
    //
    {
        ASSERT_EQ(fib_func->front().bb_id(), 0);
        ASSERT_EQ(fib_func->back().bb_id(), 3);
    }
}