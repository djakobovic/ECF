cmake_minimum_required(VERSION 2.8)
project(CEC)

set(SOURCE_FILES
    cec14_test_func.cpp
    FunctionMinEvalOp.cpp
    FunctionMinEvalOp.h
    main.cpp)

include_directories(../../)
add_executable(CEC ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(CEC ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(CEC ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()
