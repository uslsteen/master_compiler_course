cmake_minimum_required(VERSION 3.16)

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(
    FATAL_ERROR
      "In-source builds are disabled.
  Please create a subfolder and use `cmake ..` inside it.
  NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
  You must delete them, or cmake will refuse to work.")
endif()

project(leech-compiler)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O2")
endif()

include(cmake/subdirs.cmake)
include(cmake/upd_list.cmake)
include(cmake/flags.cmake)

# Flag to build tests
option(BUILD_TESTS "Build tests" OFF)

add_subdirectory(src)
add_subdirectory(3rd_party)
add_subdirectory(test)

# add_subdirectory(tools)

# Test running stuff
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

message("Libs: ${LIBLIST}")
message("Tools: ${TOOLLIST}")
message("Include directore: ${CMAKE_SOURCE_DIR}/include")

set(TARGETS)
list(APPEND TARGETS ${LIBLIST} ${TOOLLIST})

foreach(TARGET ${TARGETS})
  target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_compile_features(${TARGET} PRIVATE cxx_std_17)
  
  # NOTE: apply it since it will be released
  # apply_compiler_flags(${TARGET} PUBLIC)

  if(ENABLE_WERROR)
    target_compile_options(${TARGET} PRIVATE -Werror)
  endif()
endforeach()

foreach(TOOL ${TOOLLIST})
  target_link_libraries(${TOOL} PRIVATE ${LIBLIST})
endforeach()