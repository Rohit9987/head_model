cmake_minimum_required(VERSION 3.16)
project(headmodel_fluence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(headmodel_demo
    src/main.cc
)

target_include_directories(headmodel_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Helpful warnings
if (MSVC)
    target_compile_options(headmodel_demo PRIVATE /W4)
else()
    target_compile_options(headmodel_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()

