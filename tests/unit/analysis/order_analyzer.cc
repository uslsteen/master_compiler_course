
//! NOTE:                This relative path exists for reason
//!       get simple include and dont provide include directories in cmake file
//                            *** DONT CHANGE IT ***

#include "../graph/builder.hh"
//
#include <gtest/gtest.h>
#include <sys/types.h>

namespace jj_vm::testing {
//
class LinearOrderTest1 : public LinearOrderInterface {
protected:
    LinearOrderTest1() = default;

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

class LinearOrderTest2 : public LinearOrderInterface {
protected:
    LinearOrderTest2() = default;

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

class LinearOrderTest3 : public LinearOrderInterface {
protected:
    LinearOrderTest3() = default;

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

class LinearOrderTest4 : public LinearOrderInterface {
protected:
    LinearOrderTest4() = default;

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

class LinearOrderTest5 : public LinearOrderInterface {
protected:
    LinearOrderTest5() = default;

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

class LinearOrderTest6 : public LinearOrderInterface {
protected:
    LinearOrderTest6() = default;

    void create_test() {
        init_test(8);

        /*
            A - 0, B - 1, C - 2, D - 3, E - 4, F - 5, G - 6, H - 7
        */
        create_edge('B', 'A');
        create_edge('C', 'B');
        create_edge('D', 'B');
        create_edge('E', 'C');
        create_edge('F', 'C');
        create_edge('F', 'D');
        create_edge('G', 'F');
        create_edge('H', 'G');
        create_edge('B', 'G');
        create_edge('A', 'H');

        build();
    }
};

TEST_F(LinearOrderTest1, basic) {
   create_test();
   ASSERT_TRUE(check_order({0, 1, 5, 6, 4, 2, 3}));
}
//
TEST_F(LinearOrderTest2, basic) {
    create_test();
    ASSERT_TRUE(check_order({0, 1, 9, 6, 7, 2, 3, 4, 5, 8, 10}));
}
//
TEST_F(LinearOrderTest3, basic) {
    create_test();
    ASSERT_TRUE(check_order({0, 1, 4, 5, 7, 2, 3, 6, 8}));
}
//
TEST_F(LinearOrderTest4, basic) {
    create_test();
    //
    ASSERT_TRUE(check_order({0, 1, 3, 4, 2}));
}
// //
TEST_F(LinearOrderTest5, basic) {
    create_test();
    ASSERT_TRUE(check_order({0, 1, 2, 4, 5, 3}));
}
}  // namespace jj_vm::testing
