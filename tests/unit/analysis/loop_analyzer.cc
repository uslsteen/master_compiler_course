
//! NOTE:                This relative path exists for reason
//!       get simple include and dont provide include directories in cmake file
//                            *** DONT CHANGE IT ***

#include "../graph/builder.hh"
//
#include <gtest/gtest.h>
#include <sys/types.h>

namespace jj_vm::testing {
//

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

class AnalyzerTest2 : public LoopInterface {
protected:
    AnalyzerTest2() = default;

    void create_test() {
        init_test(11);
        //
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('J', 'B');
        create_edge('D', 'C');
        create_edge('C', 'D');
        create_edge('E', 'D');
        create_edge('F', 'E');
        create_edge('E', 'F');
        create_edge('G', 'F');
        create_edge('H', 'G');
        create_edge('I', 'G');
        create_edge('B', 'H');
        create_edge('K', 'I');
        create_edge('C', 'J');

        build();
    }
};

class AnalyzerTest3 : public LoopInterface {
protected:
    AnalyzerTest3() = default;

    void create_test() {
        init_test(9);
        //
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('E', 'B');
        create_edge('D', 'C');
        create_edge('G', 'D');
        create_edge('D', 'E');
        create_edge('F', 'E');
        create_edge('B', 'F');
        create_edge('H', 'F');
        create_edge('C', 'G');
        create_edge('I', 'G');
        create_edge('G', 'H');
        create_edge('I', 'H');

        build();
    }
};

class AnalyzerTest4 : public LoopInterface {
protected:
    AnalyzerTest4() = default;

    void create_test() {
        init_test(5);

        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('D', 'B');
        create_edge('E', 'D');
        create_edge('B', 'E');

        build();
    }
};

class AnalyzerTest5 : public LoopInterface {
protected:
    AnalyzerTest5() = default;

    void create_test() {
        init_test(6);

        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('D', 'C');
        create_edge('E', 'C');
        create_edge('D', 'E');
        create_edge('F', 'E');
        create_edge('B', 'F');

        build();
    }
};

class AnalyzerTest6 : public LoopInterface {
protected:
    AnalyzerTest6() = default;

    void create_test() {
        init_test(8);

        build();
    }
};

//
TEST_F(AnalyzerTest1, basic) {
    create_test();
    //

    //! NOTE: there is only root loop
    auto *root = get_loop(0);
    {
        EXPECT_TRUE(root->is_root());
        EXPECT_TRUE(check_header(root));
        EXPECT_TRUE(check_back_edges(root, {}));
        EXPECT_TRUE(root->outer() == nullptr);
        EXPECT_TRUE(root->inners().empty());
    }
}

TEST_F(AnalyzerTest2, basic) {
    create_test();
    //
    auto *root = get_loop(0);
    auto *loop1 = get_loop(1);
    auto *loop2 = get_loop(2);
    auto *loop3 = get_loop(4);

    //! NOTE: test root loop
    {
        EXPECT_TRUE(root->is_root());
        EXPECT_TRUE(check_header(root));
        EXPECT_TRUE(check_back_edges(root, {}));
        EXPECT_EQ(root, get_loop(8));
        EXPECT_EQ(root, get_loop(10));
        EXPECT_EQ(root->outer(), nullptr);
        EXPECT_TRUE(check_inners(root, loop1));
    }
    {
        EXPECT_FALSE(loop1->is_root());
        EXPECT_TRUE(check_header(loop1, 1));
        EXPECT_TRUE(check_back_edges(loop1, {7}));
        EXPECT_EQ(get_loop(6), loop1);
        EXPECT_EQ(get_loop(7), loop1);
        EXPECT_EQ(get_loop(9), loop1);
        EXPECT_EQ(loop1->outer(), root);
        EXPECT_TRUE(loop1->is_reducible());
        EXPECT_TRUE(check_inners(loop1, loop2));
        EXPECT_TRUE(check_inners(loop1, loop3));
    }
    {
        EXPECT_FALSE(loop2->is_root());
        EXPECT_TRUE(check_header(loop2, 2));
        EXPECT_TRUE(check_back_edges(loop2, {3}));
        EXPECT_EQ(get_loop(3), loop2);
        EXPECT_EQ(loop2->outer(), loop1);
        EXPECT_TRUE(loop2->is_reducible());
        EXPECT_TRUE(check_inners(loop2));
        EXPECT_TRUE(check_inners(loop2));
    }
    //
    {
        EXPECT_FALSE(loop3->is_root());
        EXPECT_TRUE(check_back_edges(loop3, {5}));
        EXPECT_TRUE(check_header(loop3, 4));
        EXPECT_EQ(get_loop(5), loop3);
        EXPECT_EQ(loop3->outer(), loop1);
        EXPECT_TRUE(loop3->is_reducible());
        EXPECT_TRUE(check_inners(loop3));
        EXPECT_TRUE(check_inners(loop3));
    }
}

TEST_F(AnalyzerTest3, basic) {
    create_test();
    //
    const auto *root = get_loop(0);
    const auto *loop1 = get_loop(1);
    const auto *loop2 = get_loop(2);

    {
        EXPECT_TRUE(root->is_root());
        EXPECT_TRUE(check_header(root));

        EXPECT_TRUE(check_back_edges(root, {}));

        EXPECT_EQ(get_loop(3), root);
        EXPECT_EQ(get_loop(7), root);
        EXPECT_EQ(get_loop(8), root);

        EXPECT_EQ(root->outer(), nullptr);
        EXPECT_TRUE(check_inners(root, loop1));
    }

    {
        EXPECT_FALSE(loop2->is_root());
        EXPECT_FALSE(loop2->is_reducible());

        EXPECT_TRUE(check_back_edges(loop2, {6}));
        EXPECT_TRUE(check_header(loop2, 2));
        EXPECT_EQ(get_loop(6), loop2);
        EXPECT_EQ(loop2->outer(), root);
        EXPECT_TRUE(check_inners(loop2));
    }

    {
        EXPECT_FALSE(loop1->is_root());
        EXPECT_TRUE(loop1->is_reducible());
        EXPECT_TRUE(check_header(loop1, 1));
        EXPECT_TRUE(check_back_edges(loop1, {5}));
        EXPECT_EQ(get_loop(4), loop1);
        EXPECT_EQ(get_loop(5), loop1);
        EXPECT_EQ(loop1->outer(), root);
        EXPECT_TRUE(check_inners(loop1));
    }
}

TEST_F(AnalyzerTest4, basic) {
    create_test();
    //
    const auto *root = get_loop(0);
    const auto *loop = get_loop(1);

    ASSERT_NE(root, loop);

    {
        EXPECT_TRUE(root->is_root());
        EXPECT_TRUE(check_header(root));
        EXPECT_TRUE(check_back_edges(root, {}));
        EXPECT_EQ(get_loop(2), root);
        EXPECT_EQ(root->outer(), nullptr);
        EXPECT_TRUE(check_inners(root, loop));
    }

    {
        EXPECT_FALSE(loop->is_root());
        EXPECT_TRUE(check_header(loop, 1));
        EXPECT_TRUE(check_back_edges(loop, {4}));
        EXPECT_TRUE(loop->is_reducible());
        EXPECT_EQ(get_loop(3), loop);
        EXPECT_EQ(get_loop(4), loop);
        EXPECT_EQ(loop->outer(), root);
        EXPECT_TRUE(check_inners(loop));
    }
}

TEST_F(AnalyzerTest5, basic) {
    create_test();
    //
    const auto *root = get_loop(0);
    const auto *loop = get_loop(1);

    // Assert
    ASSERT_NE(root, loop);

    {
        EXPECT_TRUE(root->is_root());
        EXPECT_TRUE(check_back_edges(root, {}));
        EXPECT_TRUE(check_header(root));
        EXPECT_EQ(get_loop(3), root);
        EXPECT_EQ(root->outer(), nullptr);
        EXPECT_TRUE(check_inners(root, loop));
    }

    {
        EXPECT_FALSE(loop->is_root());
        EXPECT_TRUE(check_header(loop, 1));
        EXPECT_TRUE(check_back_edges(loop, {5}));
        EXPECT_TRUE(loop->is_reducible());
        EXPECT_EQ(get_loop(2), loop);
        EXPECT_EQ(get_loop(4), loop);
        EXPECT_EQ(get_loop(5), loop);
        EXPECT_EQ(loop->outer(), root);
        EXPECT_TRUE(check_inners(loop));
    }
}

TEST_F(AnalyzerTest6, basic) { create_test(); }

//
}  // namespace jj_vm::testing
