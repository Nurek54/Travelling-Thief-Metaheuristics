cmake_minimum_required(VERSION 4.3)
project(travelling_thief_metaheuristics LANGUAGES CXX)

# --- Standard C++ ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Domyslnie Release, gdy nikt nie wybral trybu (np. budowanie z konsoli)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- Zrodla: main.cpp + wszystko z src/ ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(travelling_thief_metaheuristics main.cpp ${SOURCES})

# #include "ttp/Problem.h" itd.
target_include_directories(travelling_thief_metaheuristics PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Sciezki do danych i wynikow niezalezne od katalogu uruchomienia (CLion odpala exe z cmake-build-*)
target_compile_definitions(travelling_thief_metaheuristics PRIVATE
        TTP_DATA_DIR="${CMAKE_SOURCE_DIR}/data"
        TTP_RESULTS_DIR="${CMAKE_SOURCE_DIR}/results"
        TTP_TESTS_DIR="${CMAKE_SOURCE_DIR}/tests"
)

# --- Flagi kompilatora ---
if(MSVC)
    target_compile_options(travelling_thief_metaheuristics PRIVATE /W4 $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(travelling_thief_metaheuristics PRIVATE
            -Wall -Wextra -Wpedantic -Wshadow
            $<$<CONFIG:Debug>:-g -O0>
            $<$<CONFIG:Release>:-O3>
    )
endif()
