#!/bin/bash

# Crear la carpeta de compilación si no existe y entrar en ella
mkdir -p build
cd build

# Generar Makefiles y compilar
cmake ..
make

# Regresar a la carpeta principal
cd ..

# Ejecutar el programa con tus parámetros por defecto
./build/programa -genes 30 -bits 16 -poblacion 50 -generaciones 100 -funcion 4