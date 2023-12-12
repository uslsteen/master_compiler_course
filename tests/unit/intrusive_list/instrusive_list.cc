#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "intrusive_list/ilist.hh"
#include "intrusive_list/ilist_base.hh"

using namespace jj_vm::ilist_detail;

class TestNode final : public jj_vm::ilist_detail::ilist_node {
    int m_data{};

public:
    TestNode() = default;
    explicit TestNode(int data) : m_data(data) {}

    auto data() const noexcept { return m_data; }
};

// template <std::size_t size>
class IListTest1 : public ::testing::Test {};

TEST(IListTest1, empty) {
    jj_vm::ilist<TestNode> ilist{};

    EXPECT_TRUE(ilist.empty());
    EXPECT_EQ(ilist.begin(), ilist.end());
    EXPECT_EQ(ilist.size(), 0);
}

class IListTest2 : public ::testing::Test {
protected:
    IListTest2()
        : node1(std::make_unique<TestNode>(1)),
          node2(std::make_unique<TestNode>(2)) {}

    std::unique_ptr<TestNode> node1;
    std::unique_ptr<TestNode> node2;
};

TEST_F(IListTest2, push_back) {
    jj_vm::ilist<TestNode> ilist{};
    //
    //
    ilist.push_back(node1.release());
    ilist.push_back(node2.release());

    EXPECT_EQ(ilist.front().data(), 1);
    EXPECT_EQ(ilist.back().data(), 2);
}
