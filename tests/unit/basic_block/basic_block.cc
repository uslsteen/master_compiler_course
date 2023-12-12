#include "gtest/gtest.h"
#include "IR/basic_block.hh"

TEST(BasicBlock, init) {
    jj_vm::ir::BasicBlock bb{1};
}