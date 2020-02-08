cd build

conan install .. --build=missing
# Or 
# conan install -o restinio:boost_libs=static .. --build=missing
# conan install -o restinio:boost_libs=shared .. --build=missing
cmake ..
cmake --build . --config Release
# Check the example.
./bin/hello_world_minimal &