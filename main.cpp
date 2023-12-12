#include "IR/basic_block.hh"
#include "IR/function.hh"
#include "IR/instruction.hh"
#include "IR/ir_builder.hh"
//
#include <iostream>

int main() {

    jj_vm::IRBuilder builder{};
    //
    auto* fib_func = jj_vm::Function::create_function<jj_vm::Param>(
        jj_vm::Type::create<jj_vm::TypeId::I64>(), std::string{"fib"});
    //
    auto* v0 = fib_func->create<jj_vm::Param, jj_vm::Type>(jj_vm::TypeId::I32);

    auto bb0 = fib_func->create<jj_vm::BasicBlock>();
    auto bb1 = fib_func->create<jj_vm::BasicBlock>();
    //
    jj_vm::BasicBlock::link_blocks(bb1, bb0);
    //
    //
    builder.set_insert_point(bb0);
    //
    auto* v1 = builder.create<jj_vm::ConstI32>(1);
    auto* v2 = builder.create<jj_vm::ConstI64>(2);
    //
    auto branch_bb0_bb1 = builder.create<jj_vm::BranchInstr>(bb1);

    bb0->dump(std::cout);
    std::cout << bb0->size() << std::endl;
    //
    return 0;
}