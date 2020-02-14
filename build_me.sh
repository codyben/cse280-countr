#!/bin/bash
mkdir -p build/ && cd build && conan install .. --build=missing && cmake .. && cmake --build . --config Release && ./bin/hello_world_minimal