set(TARGET basic_block)
add_executable(${TARGET}_test ${TARGET}.cc)
#
UPD_LIST(${TARGET}_test TESTLIST)