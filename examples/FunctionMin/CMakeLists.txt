cmake_minimum_required(VERSION 2.8)
project(FunctionMin)

set(SOURCE_FILES
    FunctionMinEvalOp.cpp
    FunctionMinEvalOp.h
    main.cpp)

# try setting dir manually if environment variable is not set
#set (BOOST_ROOT "C:/Program Files/boost_1_74_0")

find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(../../)
add_executable(FunctionMin ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(FunctionMin ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(FunctionMin ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()
