cmake_minimum_required(VERSION 3.14)
project(sju_cpu_test)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIR /Users/kshgroup/Library/boost_1_70_0/)
set(Boost_LIBRARY_DIR /Users/kshgroup/Library/boost_1_70_0/stage/lib)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(sju_cpu_test sju_cpu_test.cpp)