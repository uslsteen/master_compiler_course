set(TARGETS peephole constant_fold checks_elimination)

foreach(TARGET ${TARGETS})
    add_executable(${TARGET}_test ${TARGET}.cc)
    UPD_LIST(${TARGET}_test TESTLIST)
    gtest_discover_tests(
        ${TARGET}_test
        EXTRA_ARGS --gtest_color=yes
        PROPERTIES LABELS unit)
endforeach()
