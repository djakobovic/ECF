cmake_minimum_required(VERSION 3.5)
project(FunctionMin)

set(SOURCE_FILES
    FunctionMinEvalOp.cpp
    FunctionMinEvalOp.h
    main.cpp)

include_directories(../../)
add_executable(FunctionMin ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(FunctionMin ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(FunctionMin ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()
