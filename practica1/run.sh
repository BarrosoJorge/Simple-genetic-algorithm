#!/usr/bin/env bash
set -e
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
  mkdir $BUILD_DIR
fi
cd $BUILD_DIR

cmake ..

cmake --build .

./programa -gaussianas 10 -sup 8 3 1 -inf -8 0.2 0 -poblacion 200 -generaciones 1000 -cruza 0.85 -mutacion 0.02 -bits 8
