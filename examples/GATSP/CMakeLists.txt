cmake_minimum_required(VERSION 2.8)
project(GATSP)

set(SOURCE_FILES
    main.cpp
    TSPEvalOp.cpp
    TSPEvalOp.h)

include_directories(../../)
add_executable(GATSP ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(GATSP ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(GATSP ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()
