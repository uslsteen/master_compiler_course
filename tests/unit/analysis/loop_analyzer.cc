
//! NOTE:                This relative path exists for reason
//!       get simple include and dont provide include directories in cmake file
//                            *** DONT CHANGE IT ***
#include "analysis/loop_analyzer.hh"

#include "../graph/builder.hh"
//
#include <gtest/gtest.h>

namespace jj_vm::testing {
//
class LoopInterface : public TestBuilder {
protected:
    LoopInterface() = default;

    void build() {
        m_tree = jj_vm::analysis::loop::LoopTreeBuilder<jj_vm::graph::BBGraph>::build(
            m_func->bb_graph());
    }

    bool check_header() { return true; }

    bool check_back_edges() { return true; }

    bool check_inners() { return true; }

    //
    jj_vm::analysis::loop::LoopTree<jj_vm::graph::BBGraph> m_tree;
};

//
class AnalyzerTest1 : public LoopInterface {
protected:
    AnalyzerTest1() = default;

    void create_test() {
        init_test(7);
        //
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('F', 'B');
        create_edge('E', 'F');
        create_edge('G', 'F');
        create_edge('D', 'G');
        create_edge('D', 'E');
        create_edge('D', 'C');

        build();
    }
};

//
TEST_F(AnalyzerTest1, basic) {
    //
}
}  // namespace jj_vm::testing