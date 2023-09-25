SUBDIRS(DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set(LIBLIST ${DIRS})

foreach(TARGET ${DIRS})
    add_subdirectory(${TARGET})
endforeach() 

set(LIBLIST ${LIBLIST} PARENT_SCOPE)