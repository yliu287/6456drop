cmake_minimum_required(VERSION 2.8.9)

project(exp2)

## toggle this to use vtune itt notify instrumentation
SET(USE_VTUNE "1") 
SET(USE_INIT_ELEMENTS "0")

# -- apply to all configurations --- #
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -Wall -Wextra -Werror -pthread -O2")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_C_COMPILER "gcc")

include_directories(
  ./
)

set(COMMON_LIB
    dl
    )
  
if (USE_VTUNE)
    include_directories(
            /data/intel/vtune_profiler/include   # for vtune instrumentation
    )
    
    ADD_LIBRARY(ittnotify STATIC IMPORTED)
    SET_TARGET_PROPERTIES(ittnotify PROPERTIES IMPORTED_LOCATION /data/intel/vtune_profiler/lib64/libittnotify.a)
    SET (COMMON_LIB ittnotify ${COMMON_LIB})
    
    add_definitions(-DUSE_VTUNE)    
endif()

if (USE_INIT_ELEMENTS)
  add_definitions(-DUSE_INIT_ELEMENTS)
endif()

add_definitions(-DDEBUG)

file(GLOB HEADERS
"./*.h"
)

file(GLOB COMMON_SOURCES
  ${HEADERS}
  SortedList.c 
  measure.c 
  common.c)

# ------------------------------------- #
# all executables  
# ------------------------------------- #

# biglock
add_executable(list
  main.c    
  ${COMMON_SOURCES}
)

TARGET_LINK_LIBRARIES(list
        ${COMMON_LIB}
        )      

