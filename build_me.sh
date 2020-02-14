#!/bin/bash
conan remote add stiffstream https://api.bintray.com/conan/stiffstream/public
# Add remote for conan to find RESTinio's dependencies.
conan remote add public-conan https://api.bintray.com/conan/bincrafters/public-conan  
mkdir -p build/ && cd build && conan install .. --build=missing && cmake .. && cmake --build . --config Release && ./bin/countr_server