#!/bin/bash

gcc -Ilib/pdi src/pdi/pdi.c -c -o pdi_obj.o
gcc -Ilib/pdi src/pdi/circle_detection.c -c -o cdet_obj.o

g++ -std=c++20 -Ilib app/main.cpp \
    src/ga/sga.cpp src/experiments/program.cpp \
    src/io/reader.cpp src/math/dataset.cpp \
    src/math/gaussian.cpp src/math/loss.cpp src/math/model.cpp \
    pdi_obj.o cdet_obj.o \
    -o programa -lm

echo "Compilado y ejecutando"
./programa
