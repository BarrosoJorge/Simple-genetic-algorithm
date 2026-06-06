cmake_minimum_required(VERSION 3.10)
project(SimpleGeneticAlgorithm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(core
    src/ga/sga.cpp
    src/io/reader.cpp
    src/experiments/program.cpp 
    src/math/dataset.cpp 
    src/math/gaussian.cpp 
    src/math/loss.cpp 
    src/math/model.cpp 
)

target_include_directories(core PUBLIC lib)

add_executable(programa app/main.cpp)

target_link_libraries(programa core)
