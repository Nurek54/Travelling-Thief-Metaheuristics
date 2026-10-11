cmake_minimum_required(VERSION 4.3)
project(travelling_thief_metaheuristics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(travelling_thief_metaheuristics main.cpp ${SOURCES})
target_include_directories(travelling_thief_metaheuristics PRIVATE include)
target_compile_options(travelling_thief_metaheuristics PRIVATE -Wall -Wextra $<$<CONFIG:Release>:-O3>)
