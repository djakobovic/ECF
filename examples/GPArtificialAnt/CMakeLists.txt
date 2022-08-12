cmake_minimum_required(VERSION 2.8)
project(GPArtificialAnt)

set(SOURCE_FILES
    AntEvalOp.cpp
    AntEvalOp.h
    IfFoodAhead.h
    main.cpp
    MoveAhead.h
    Prog2.h
    Prog3.h
    TurnLeft.h
    TurnRight.h)

# try setting dir manually if environment variable is not set
#set (BOOST_ROOT "C:/Program Files/boost_1_74_0")

find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(../../)
add_executable(GPArtificialAnt ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(GPArtificialAnt ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(GPArtificialAnt ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()