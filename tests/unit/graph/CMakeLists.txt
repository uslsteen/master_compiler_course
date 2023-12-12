set(TARGET dfs)
add_executable(${TARGET}_test ${TARGET}.cc)

#
UPD_LIST(${TARGET}_test TESTLIST)

set(TARGET dom3)
add_executable(${TARGET}_test ${TARGET}.cc)
#
UPD_LIST(${TARGET}_test TESTLIST)