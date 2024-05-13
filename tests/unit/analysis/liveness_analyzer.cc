//! NOTE:                This relative path exists for reason
//!       get simple include and dont provide include directories in cmake file
//                            *** DONT CHANGE IT ***

#include "../graph/builder.hh"
//
#include <gtest/gtest.h>
#include <sys/types.h>

#include "IR/basic_block.hh"
#include "IR/function.hh"
#include "IR/instruction.hh"
#include "IR/ir_builder.hh"

namespace jj_vm::testing {
//

class LivenessTest1 : public LivenessInterface {
protected:
    std::vector<jj_vm::ir::Instr*> m_instr{};

    LivenessTest1()
        : LivenessInterface({2, 4, 6, 10, 10, 12, 14, 18, 20, 26}, {{2, 24},
                                                                    {4, 10},
                                                                    {6, 26},
                                                                    {10, 26},
                                                                    {10, 20},
                                                                    {12, 14},
                                                                    {14, 14},
                                                                    {18, 20},
                                                                    {20, 22},
                                                                    {26, 28}}) {
    }

    /*
        lecture_test() {                    Live Num
        0:                                     0
          v0 = i64 1                           2
          v1 = i64 10                          4
          v2 = i64 20                          6
          jmp bb1                              8

        1:                                     10
          v3 = phi i32 [v0, bb0], [v7, bb2]    10
          v4 = phi i64 [v1, bb0], [v8, bb2]    10
          v5 = cmp EQ v3, v2                   12
          if v5, bb3, bb2                      14


        2:                                     16
          v7 = MUL i64 v3, v4                  18
          v8 = SUB i64 v4, v0                  20
          jmp bb1                              22

        3:                                     24
          v9 = ADD i64 v2, v3                  26
          RET v9                               28
        }
    */
    void make_ir() {
        jj_vm::ir::IRBuilder builder{};
        init_test(4);

        auto bb0 = get_bb(0);
        auto bb1 = get_bb(1);
        auto bb2 = get_bb(2);
        auto bb3 = get_bb(3);

        builder.set_insert_point(bb0);

        auto* v0 = builder.create<jj_vm::ir::ConstI64>(1);
        auto* v1 = builder.create<jj_vm::ir::ConstI64>(10);
        auto* v2 = builder.create<jj_vm::ir::ConstI64>(20);
        auto bb0_to_bb1 = builder.create<jj_vm::ir::BranchInstr>(bb1);

        builder.set_insert_point(bb1);

        auto* v3 = builder.create<jj_vm::ir::PhiInstr>(jj_vm::ir::TypeId::I64);
        auto* v4 = builder.create<jj_vm::ir::PhiInstr>(jj_vm::ir::TypeId::I64);
        auto* v5 =
            builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::EQ, v4, v0);
        auto* v6 = builder.create<jj_vm::ir::IfInstr>(bb3, bb2, v5);

        v3->add_node(std::make_pair(v0, bb0));
        v4->add_node(std::make_pair(v1, bb0));

        builder.set_insert_point(bb2);

        auto* v7 =
            builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::MUL, v3, v4);
        auto* v8 =
            builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::SUB, v4, v0);
        auto bb2_to_bb1 = builder.create<jj_vm::ir::BranchInstr>(bb1);

        v3->add_node(std::make_pair(v7, bb2));
        v4->add_node(std::make_pair(v8, bb2));

        builder.set_insert_point(bb3);

        auto* v9 =
            builder.create<jj_vm::ir::BinInstr>(jj_vm::ir::Opcode::ADD, v2, v3);
        auto* v10 = builder.create<jj_vm::ir::RetInstr>(v9);

        //

        for (auto&& value : std::vector<jj_vm::ir::Value*>{v0, v1, v2, v3, v4,
                                                           v5, v6, v7, v8, v9})
            m_instr.push_back(static_cast<jj_vm::ir::Instr*>(value));
    }

    void create_test() {
        make_ir();
        build();
    }
};

TEST_F(LivenessTest1, lecture) {
    create_test();

    EXPECT_EQ(m_instr.size(), ref_live_nums.size());

    for (std::size_t i = 0; i < m_instr.size(); ++i)
        EXPECT_EQ(m_instr.at(i)->live(), ref_live_nums[i]);

    for (std::size_t i = 0; i < m_instr.size(); ++i) {
        EXPECT_EQ(*m_analyzer.get_interval(m_instr.at(i)), ref_life_ranges[i]);
    }
}
}  // namespace jj_vm::testing
