cmake_minimum_required(VERSION 3.10)
project(SimpleGeneticAlgorithm)

# Usar el estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Definir los archivos fuente
set(SOURCES main.cpp asignacion.cpp sga.cpp)

# Crear el ejecutable final llamado "programa"
add_executable(programa ${SOURCES})