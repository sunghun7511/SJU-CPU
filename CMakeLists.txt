cmake_minimum_required(VERSION 3.14)
project(SJUCPU)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIR /Users/kshgroup/Library/boost_1_70_0/)
set(Boost_LIBRARY_DIR /Users/kshgroup/Library/boost_1_70_0/stage/lib)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.70 COMPONENTS system filesystem program_options thread regex unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(include/)
add_executable(SJUCPU src/main.cpp src/sju_cpu.cpp src/sju_instruction_parser.cpp src/sju_assembler.cpp src/sju_debugger.cpp include/include.hpp include/sju_cpu.hpp include/sju_instruction_parser.hpp include/model.hpp include/sju_assembler.hpp include/sju_debugger.hpp)
target_link_libraries(SJUCPU ${Boost_LIBRARIES})

add_subdirectory(test/)