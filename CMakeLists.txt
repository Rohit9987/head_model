cmake_minimum_required(VERSION 3.16)
project(headmodel_fluence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(headmodel_demo
	src/main.cc src/HeadModel.cpp
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


# Headmodel library
add_library(head_model src/HeadModel.cpp)
target_include_directories(head_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(head_model PUBLIC cxx_std_17)
