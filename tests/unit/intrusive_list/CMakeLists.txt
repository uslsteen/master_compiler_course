set(TARGET instrusive_list)
add_executable(${TARGET}_test ${TARGET}.cc)
#
UPD_LIST(${TARGET}_test TESTLIST)
gtest_discover_tests(
    ${TARGET}_test
    EXTRA_ARGS --gtest_color=yes
    PROPERTIES LABELS unit)
