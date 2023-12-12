#include "IR/basic_block.hh"
#include "IR/function.hh"
#include "IR/instruction.hh"
#include "IR/ir_builder.hh"
//
#include <iostream>

int main() {

    jj_ir::IRBuilder builder{};
    //
    auto* fib_func = jj_ir::Function::create_function<jj_ir::Param>(
        jj_ir::Type::create<jj_ir::TypeId::I64>(), std::string{"fib"});
    //
    auto* v0 = fib_func->create<jj_ir::Param, jj_ir::Type>(jj_ir::TypeId::I32);

    auto bb0 = fib_func->create<jj_ir::BasicBlock>();
    auto bb1 = fib_func->create<jj_ir::BasicBlock>();
    //
    jj_ir::BasicBlock::link_blocks(bb1, bb0);
    //
    //
    builder.set_insert_point(bb0);
    //
    auto* v1 = builder.create<jj_ir::ConstI32>(1);
    auto* v2 = builder.create<jj_ir::ConstI64>(2);
    //
    auto branch_bb0_bb1 = builder.create<jj_ir::BranchInstr>(bb1);

    bb0->dump(std::cout);
    std::cout << bb0->size() << std::endl;
    //
    return 0;
}