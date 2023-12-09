#include <sstream>
#include <string_view>
#include <vector>
//
#include <gtest/gtest.h>
//
#include "builder.hh"
#include "graph/dfs.hh"

namespace jj_ir::testing {

TEST_F(DFSTest1, dfs) {
    //
    create_test();
    //
    auto&& graph = m_func->bb_graph();

    auto postorder_res = jj_ir::graph::deep_first_search_postoder(graph);
    auto preorder_res = jj_ir::graph::deep_first_search_preoder(graph);
    //
    ASSERT_TRUE(check_order(postorder_res, postorder_ref));
    ASSERT_TRUE(check_order(preorder_res, preorder_ref));
    //
    std::filesystem::path path{"example1.dot"};
    graph.dot_dump(path, "Example1");
}

TEST_F(DFSTest2, dfs) {
    //
    create_test();
    auto&& graph = m_func->bb_graph();
    //
    auto postorder_res = jj_ir::graph::deep_first_search_postoder(graph);
    auto preorder_res = jj_ir::graph::deep_first_search_preoder(graph);
    //
    ASSERT_TRUE(check_order(preorder_res, preorder_ref));
    ASSERT_TRUE(check_order(postorder_res, postorder_ref));
    //
    std::filesystem::path path{"example2.dot"};
    graph.dot_dump(path, "Example2");
}

TEST_F(DFSTest3, dfs) {
    //
    create_test();
    //
    auto&& graph = m_func->bb_graph();

    auto postorder_res = jj_ir::graph::deep_first_search_postoder(graph);
    auto preorder_res = jj_ir::graph::deep_first_search_preoder(graph);

    ASSERT_TRUE(check_order(postorder_res, postorder_ref));
    ASSERT_TRUE(check_order(preorder_res, preorder_ref));

    std::filesystem::path path{"example3.dot"};
    graph.dot_dump(path, "Example3");
}

}  // namespace jj_ir::testing
