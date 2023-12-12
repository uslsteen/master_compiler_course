#include <gtest/gtest.h>
//
#include "builder.hh"
#include "graph/dfs.hh"

namespace jj_ir::testing {

class Dom3Tree1 : public DominatorInterface {
protected:
    Dom3Tree1() = default;

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

class Dom3Tree2 : public DominatorInterface {
protected:
    Dom3Tree2() = default;

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

class Dom3Tree3 : public DominatorInterface {
protected:
    Dom3Tree3() = default;

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

TEST_F(Dom3Tree1, basic) {
    create_test();
    for (std::size_t i = 1; i < m_basic_blocks.size(); ++i)
        ASSERT_TRUE(is_dominator(0, i));

    ASSERT_TRUE(is_dominator(0, 1));

    ASSERT_TRUE(is_dominator(1, 2));
    ASSERT_TRUE(is_dominator(1, 3));
    ASSERT_TRUE(is_dominator(1, 4));
    ASSERT_TRUE(is_dominator(1, 5));
    ASSERT_TRUE(is_dominator(1, 6));

    ASSERT_TRUE(is_dominator(5, 4));
    ASSERT_TRUE(is_dominator(5, 6));
}

TEST_F(Dom3Tree2, basic) {
    create_test();
    for (std::size_t i = 1; i < m_basic_blocks.size(); ++i)
        ASSERT_TRUE(is_dominator(0, i));

    ASSERT_TRUE(is_dominator(0, 1));

    ASSERT_TRUE(is_dominator(1, 2));
    ASSERT_TRUE(is_dominator(1, 3));
    ASSERT_TRUE(is_dominator(1, 4));
    ASSERT_TRUE(is_dominator(1, 5));
    ASSERT_TRUE(is_dominator(1, 6));
    ASSERT_TRUE(is_dominator(1, 7));
    ASSERT_TRUE(is_dominator(1, 8));
    ASSERT_TRUE(is_dominator(1, 9));
    ASSERT_TRUE(is_dominator(1, 10));

    ASSERT_TRUE(is_dominator(2, 3));

    ASSERT_TRUE(is_dominator(3, 4));
    
    ASSERT_TRUE(is_dominator(4, 5));
    
    ASSERT_TRUE(is_dominator(5, 6));
    ASSERT_TRUE(is_dominator(6, 7));
    ASSERT_TRUE(is_dominator(6, 8));
    
    ASSERT_TRUE(is_dominator(8, 10));
}

TEST_F(Dom3Tree3, basic) {
    create_test();
    for (std::size_t i = 1; i < m_basic_blocks.size(); ++i)
        ASSERT_TRUE(is_dominator(0, i));

    ASSERT_TRUE(is_dominator(1, 2));
    ASSERT_TRUE(is_dominator(1, 3));
    ASSERT_TRUE(is_dominator(1, 4));
    ASSERT_TRUE(is_dominator(1, 6));
    ASSERT_TRUE(is_dominator(1, 8));

    ASSERT_TRUE(is_dominator(4, 5));

    ASSERT_TRUE(is_dominator(5, 7));
}

}  // namespace jj_ir::testing