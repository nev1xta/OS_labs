cmake_minimum_required(VERSION 3.10)
project(Lab1)

set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
endif()

include_directories(include)

add_executable(parent src/parent.cpp src/systemCall.cpp)
add_executable(child1 src/child1.cpp src/systemCall.cpp)
add_executable(child2 src/child2.cpp src/systemCall.cpp)

set_target_properties(parent child1 child2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)