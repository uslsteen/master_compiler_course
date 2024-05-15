include(GoogleTest)

SUBDIRS(DIRS ${CMAKE_CURRENT_SOURCE_DIR})
#
set(TESTLIST)

#
foreach(DIR ${DIRS})
    add_subdirectory(${DIR})
endforeach()

message(STATUS "Collected unit tests: ${TESTLIST}")

foreach(TEST ${TESTLIST})
target_include_directories(${TEST} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${TEST} PRIVATE GTest::gtest_main)
    target_link_libraries(${TEST} PRIVATE ${LIBLIST})
    target_compile_features(${TEST} PRIVATE cxx_std_17)
endforeach()
