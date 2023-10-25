set(TARGET instrusive_list)
add_executable(${TARGET}_test ${TARGET}.cc)
#
UPD_LIST(${TARGET}_test TESTLIST)