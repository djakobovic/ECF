cmake_minimum_required(VERSION 3.6)
project(COCO)

set(SOURCE_FILES
    bbob/bbobStructures.h
    bbob/benchmarks.c
    bbob/benchmarks.h
    bbob/benchmarksdeclare.c
    bbob/benchmarksdeclare.h
    bbob/benchmarkshelper.c
    bbob/benchmarkshelper.h
    bbob/benchmarksnoisy.c
    bbob/benchmarksnoisy.h
    bbob/dirOK.c
    bbob/fgeneric.c
    bbob/MY_OPTIMIZER.c
    FunctionMinEvalOp.cpp
    FunctionMinEvalOp.h
    main.cpp)

include_directories(../../)
add_executable(COCO ${SOURCE_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(COCO ${CMAKE_CURRENT_SOURCE_DIR}/../../debug/libECF.a)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(COCO ${CMAKE_CURRENT_SOURCE_DIR}/../../release/libECF.a)
endif()
