cmake_minimum_required(VERSION 3.10)
project(SimpleGeneticAlgorithm)

# Usar el estándar C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_library(core
    src/ga/sga.cpp
    src/io/reader.cpp
)
target_include_directories(core PUBLIC lib)
# Definir los archivos fuente
add_executable(programa app/main.cpp )

# Crear el ejecutable final llamado "programa"
target_include_directories(programa core)
