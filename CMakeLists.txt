cmake_minimum_required(VERSION 4.3)
project(travelling_thief_metaheuristics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# domyslnie release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# main + wszystko z src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(travelling_thief_metaheuristics main.cpp ${SOURCES})

target_include_directories(travelling_thief_metaheuristics PRIVATE ${CMAKE_SOURCE_DIR}/include)

# sciezki do data i results niezalezne od folderu z exe
target_compile_definitions(travelling_thief_metaheuristics PRIVATE
        TTP_DATA_DIR="${CMAKE_SOURCE_DIR}/data"
        TTP_RESULTS_DIR="${CMAKE_SOURCE_DIR}/results"
        TTP_TESTS_DIR="${CMAKE_SOURCE_DIR}/tests"
)

if(MSVC)
    target_compile_options(travelling_thief_metaheuristics PRIVATE /W4 $<$<CONFIG:Release>:/O2>)
else()
    target_compile_options(travelling_thief_metaheuristics PRIVATE
            -Wall -Wextra -Wpedantic -Wshadow
            $<$<CONFIG:Debug>:-g -O0>
            $<$<CONFIG:Release>:-O3>
    )
endif()

# zeby exe dzialal tez poza clionem
if(MINGW)
    target_link_options(travelling_thief_metaheuristics PRIVATE -static-libgcc -static-libstdc++)
endif()

# libwinpthread nie da sie statycznie wiec kopiujemy dll obok exe
if(MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    if(EXISTS "${MINGW_BIN_DIR}/libwinpthread-1.dll")
        add_custom_command(TARGET travelling_thief_metaheuristics POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${MINGW_BIN_DIR}/libwinpthread-1.dll"
                        $<TARGET_FILE_DIR:travelling_thief_metaheuristics>)
    endif()
endif()
