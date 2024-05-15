#pragma once

#include "IR/function.hh"
#include "IR/ir_builder.hh"
//
#include <memory>
#include <vector>

namespace jj_vm::passes {

struct Pass {
    virtual void run(jj_vm::ir::Function *fn) {}
};

class PassManager {
private:
    jj_vm::ir::Function *m_fn;
    std::vector<std::unique_ptr<Pass>> m_passes;
    //
public:
    PassManager(jj_vm::ir::Function *fn) : m_fn(fn) {}

    void register_pass(std::unique_ptr<Pass> pass) {
        m_passes.emplace_back(std::move(pass));
    }

    //
    void run() {
        for (auto &&pass : m_passes) pass->run(m_fn);
    }
};
}  // namespace jj_vm::passes
