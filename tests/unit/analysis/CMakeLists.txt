set(TARGETS reg_alloc_analyzer loop_analyzer order_analyzer liveness_analyzer)

foreach(TARGET ${TARGETS})
    add_executable(${TARGET}_test ${TARGET}.cc)
    UPD_LIST(${TARGET}_test TESTLIST)
endforeach()
