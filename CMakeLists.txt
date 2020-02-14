cmake_minimum_required(VERSION 2.8)

project(cse280-countr)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(restinio CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(countr_server countr_server.cpp)

target_link_libraries(countr_server restinio::restinio)
target_link_libraries(countr_server ${CONAN_LIBS})

if(WIN32)
    target_link_libraries(countr_server wsock32 ws2_32)
endif()
