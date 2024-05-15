#include "opt_passes/inlining.hh"

#include <gtest/gtest.h>

#include <cstdint>

#include "../graph/builder.hh"

using namespace jj_vm::ir;

namespace jj_vm::testing {

class InliningTest1 : public ::testing::Test {
protected:
    //
    Function m_caller{};
    Function m_callee{};
    //
    jj_vm::passes::Inlining m_pass;
    IRBuilder m_builder{};
    //
    InliningTest1()
        : m_caller(Function{Type::create<jj_vm::ir::TypeId::I64>(), "caller"}),
          m_callee(Function{Type::create<jj_vm::ir::TypeId::I64>(), "callee"}) {
    }

    /*
    Caller graph
    bb0:
        v0: i64 = CONST 1;
        v1: i64 = CONST 5;
        BRANCH -> bb1;

    bb1:
        v2: i64 = ADD v0, v1;
        v3: i64 = CALL func v2, v0;
        v4: i64 = SUB  v3, v0;
        ret v4;
    */
    void init_caller() {
        auto* bb0 = m_caller.create<BasicBlock>();
        auto* bb1 = m_caller.create<BasicBlock>();
        //
        m_builder.set_insert_point(bb0);
        auto* v0 = m_builder.create<ConstI64>(1);
        auto* v1 = m_builder.create<ConstI64>(5);
        m_builder.create<BranchInstr>(bb1);

        m_builder.set_insert_point(bb1);
        auto* v2 = m_builder.create<BinInstr>(Opcode::ADD, v0, v1);
        auto* v3 = m_builder.create<CallInstr>(
            Type::create<jj_vm::ir::TypeId::I64>(), &m_callee);
        v3->add_arg(v2);
        v3->add_arg(v0);
        //
        auto v4 = m_builder.create<BinInstr>(Opcode::SUB, v3, v0);
        //
        m_builder.create<RetInstr>(v4);
    }

    /*
     Callee graph
     bb0:
         v0: i64 = PARAM x;
         v1: i64 = PARAM y;
         v2: i64 = CONST 1;
         BRANCH bb1;

     bb1:
         v3: i1 = EQ v0, v1;
         if (v3, bb2, bb3);

     bb2:
         v4: i64 = ADD v0, v2;
         ret v4;

     bb3:
         v5: i64 = SUB v1, v2;
         RET v5;
    */
    void init_callee() {
        auto* bb0 = m_callee.create<BasicBlock>();
        auto* bb1 = m_callee.create<BasicBlock>();
        auto* bb2 = m_callee.create<BasicBlock>();
        auto* bb3 = m_callee.create<BasicBlock>();

        m_builder.set_insert_point(bb0);

        auto v0 = m_builder.create<ParamInstr>(
            "x", Type::create<jj_vm::ir::TypeId::I64>());
        auto v1 = m_builder.create<ParamInstr>(
            "y", Type::create<jj_vm::ir::TypeId::I64>());
        auto v2 = m_builder.create<ConstI64>(1);
        m_builder.create<BranchInstr>(bb1);

        m_builder.set_insert_point(bb1);
        auto v3 = m_builder.create<BinInstr>(Opcode::EQ, v0, v1);
        auto if_bb1 = m_builder.create<IfInstr>(bb2, bb3, v3);

        m_builder.set_insert_point(bb2);
        auto v4 = m_builder.create<BinInstr>(Opcode::ADD, v0, v2);
        m_builder.create<RetInstr>(v4);

        m_builder.set_insert_point(bb3);
        auto v5 = m_builder.create<BinInstr>(Opcode::SUB, v1, v2);
        m_builder.create<RetInstr>(v5);
    }

    //
    void run() { m_pass.run(&m_caller); }

    void build() {
        init_caller();
        init_callee();
    }
};

//
TEST_F(InliningTest1, lecture) {
    build();
    run();
    //
    std::vector<jj_vm::ir::BasicBlock*> bbs{};
    bbs.resize(m_caller.size());

    for (auto&& bb : m_caller) bbs[bb.bb_id()] = &bb;

    //
    auto* bb0 = bbs.at(0);
    auto* bb1 = bbs.at(1);
    auto* bb2 = bbs.at(2);
    auto* bb3 = bbs.at(3);
    auto* bb4 = bbs.at(4);
    auto* bb5 = bbs.at(5);

    {
        // bb0:
        //     v0: i64 = CONST 1;
        //     v1: i64 = CONST 5;
        //     BRANCH -> bb1;
        EXPECT_EQ(bb0->size(), 3);

        std::vector<jj_vm::ir::Instr*> instrs{};
        for (auto&& it : *bb0) instrs.push_back(&it);
        EXPECT_EQ(bb0->size(), instrs.size());

        EXPECT_EQ(instrs[0]->opcode(), jj_vm::ir::Opcode::CONST);
        EXPECT_EQ(instrs[1]->opcode(), jj_vm::ir::Opcode::CONST);
        EXPECT_EQ(instrs[2]->opcode(), jj_vm::ir::Opcode::BRANCH);

        EXPECT_EQ(static_cast<jj_vm::ir::BranchInstr*>(instrs[2])->dst(), bb1);
    }
    {
        // bb1:
        //     v3: i64 = ADD v0, v1;
        //     v2: i64 = CONST 1;
        //     BRANCH -> bb2;
        EXPECT_EQ(bb1->size(), 3);

        std::vector<jj_vm::ir::Instr*> instrs{};
        for (auto&& it : *bb1) instrs.push_back(&it);
        EXPECT_EQ(bb1->size(), instrs.size());

        EXPECT_EQ(instrs[0]->opcode(), jj_vm::ir::Opcode::ADD);
        EXPECT_EQ(instrs[1]->opcode(), jj_vm::ir::Opcode::CONST);
        EXPECT_EQ(instrs[2]->opcode(), jj_vm::ir::Opcode::BRANCH);

        EXPECT_EQ(static_cast<jj_vm::ir::BranchInstr*>(instrs[2])->dst(), bb2);
    }
    {
        //  bb2:
        //      v4: i1 = EQ v0, v1;
        //      if (v4, bb3, bb4);
        EXPECT_EQ(bb2->size(), 2);

        std::vector<jj_vm::ir::Instr*> instrs{};
        for (auto&& it : *bb2) instrs.push_back(&it);
        EXPECT_EQ(bb2->size(), instrs.size());

        EXPECT_EQ(instrs[0]->opcode(), jj_vm::ir::Opcode::EQ);
        EXPECT_EQ(instrs[1]->opcode(), jj_vm::ir::Opcode::IF);
        //
        auto if_instr = static_cast<jj_vm::ir::IfInstr*>(instrs[1]);

        EXPECT_EQ(if_instr->cond(), instrs[0]);
        EXPECT_EQ(if_instr->true_bb(), bb3);
        EXPECT_EQ(if_instr->false_bb(), bb4);
    }
    {
        //  bb3:
        //      v5: i64 = ADD v0, v2;
        //      BRANCH -> bb5;
        EXPECT_EQ(bb3->size(), 2);

        std::vector<jj_vm::ir::Instr*> instrs{};
        for (auto&& it : *bb3) instrs.push_back(&it);
        EXPECT_EQ(bb3->size(), instrs.size());

        auto opcode = instrs[1]->opcode();

        EXPECT_EQ(instrs[0]->opcode(), jj_vm::ir::Opcode::ADD);
        EXPECT_EQ(instrs[1]->opcode(), jj_vm::ir::Opcode::BRANCH);

        EXPECT_EQ(static_cast<jj_vm::ir::BranchInstr*>(instrs[1])->dst(), bb5);
    }
    {
        //  bb4:
        //      v6: i64 = SUB v1, v2;
        //      BRANCH -> bb5;
        EXPECT_EQ(bb4->size(), 2);

        std::vector<jj_vm::ir::Instr*> instrs{};
        for (auto&& it : *bb4) instrs.push_back(&it);
        EXPECT_EQ(bb4->size(), instrs.size());

        auto opcode = instrs[1]->opcode();

        EXPECT_EQ(instrs[0]->opcode(), jj_vm::ir::Opcode::SUB);
        EXPECT_EQ(instrs[1]->opcode(), jj_vm::ir::Opcode::BRANCH);

        EXPECT_EQ(static_cast<jj_vm::ir::BranchInstr*>(instrs[1])->dst(), bb5);
    }
    {
        // bb5
        //      v7: i64 = PHI [bb4, v6], [bb3, v5]
        //      v8: i64 = SUB  v3, v0;
        //      ret v8;
        EXPECT_EQ(bb5->size(), 3);

        std::vector<jj_vm::ir::Instr*> instrs{};
        for (auto&& it : *bb5) instrs.push_back(&it);
        EXPECT_EQ(bb5->size(), instrs.size());

        EXPECT_EQ(instrs[0]->opcode(), jj_vm::ir::Opcode::PHI);
        EXPECT_EQ(instrs[1]->opcode(), jj_vm::ir::Opcode::SUB);
        EXPECT_EQ(instrs[2]->opcode(), jj_vm::ir::Opcode::RET);

        auto* phi_node = static_cast<jj_vm::ir::PhiInstr*>(instrs[0]);
        std::vector<jj_vm::ir::PhiInstr::phi_var_pair> phi_vars{};
        for (auto&& var : phi_node->vars()) phi_vars.push_back(var);

        jj_vm::ir::Instr* v5 = &(bb3->front());
        jj_vm::ir::Instr* v6 = &(bb4->front());
        //
        EXPECT_EQ(phi_vars[0].first, v6);
        EXPECT_EQ(phi_vars[1].first, v5);

        EXPECT_EQ(phi_vars[0].second, bb4);
        EXPECT_EQ(phi_vars[1].second, bb3);
    }
}
}  // namespace jj_vm::testing
