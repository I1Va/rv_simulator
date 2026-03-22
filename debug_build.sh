#!/bin/bash

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=OFF
cmake --build build -j 8
# ./build/rv_simulator

# ./build.sh && ./build/rv_simulator -s 1 -i examples/initial_state -f examples/final_state examples/sum.elf
