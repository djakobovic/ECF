cmake_minimum_required(VERSION 2.8)
project(GPSymbReg)

set(SOURCE_FILES
    main.cpp
    SymbRegEvalOp.cpp
    SymbRegEvalOp.h)

include_directories(../../)
add_executable(GPSymbReg ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(GPSymbReg ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(GPSymbReg ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()