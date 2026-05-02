#!/usr/bin/env bash
set -e
BUILD_DIR="build"
if [ ! -d "$BUILD_DIR" ]; then
  mkdir $BUILD_DIR
fi
cd $BUILD_DIR

cmake ..

cmake --build .

./programa -gaussianas 1 -sup 5 1 0.3 -inf 0 0 0.05 -poblacion 10 -generaciones 1 -cruza 0.85 -mutacion 0.02 -bits 8
