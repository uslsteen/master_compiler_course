#include "gtest/gtest.h"
#include "IR/ir_builder.hh"
#include "IR/basic_block.hh"
// 

TEST(BasicBlock, init) {
    jj_ir::BasicBlock bb{1};
    jj_ir::IRBuilder builder{&bb};
}