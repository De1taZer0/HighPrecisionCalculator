cmake_minimum_required(VERSION 3.19)

project(Calculator)

set(CMAKE_CXX_STANDARD 17)

add_library(Calculator STATIC
        src/HighPrecision.hh
        src/HighPrecision.cc
)

target_include_directories(Calculator PUBLIC src/)

target_compile_definitions(Calculator PUBLIC _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING)

add_executable(HighPrecisionCalculator main/main.cc)

target_link_libraries(HighPrecisionCalculator Calculator)

if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
endif()
