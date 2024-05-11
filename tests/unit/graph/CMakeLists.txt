set(TARGETS dfs dom3)

foreach(TARGET ${TARGETS})
    add_executable(${TARGET}_test ${TARGET}.cc)
    UPD_LIST(${TARGET}_test TESTLIST)
endforeach()
