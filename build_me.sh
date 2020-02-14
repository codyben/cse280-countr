#!/bin/bash
cmake build/ && cmake --build build/ --config Release && ./build/bin/server
