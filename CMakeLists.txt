cmake_minimum_required(VERSION 3.16)
project(headmodel_fluence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP)
add_executable(headmodel_demo
	src/main.cc src/HeadModel.cpp
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(headmodel_demo PRIVATE OpenMP::OpenMP_CXX)
endif()

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

if(OpenMP_CXX_FOUND)
    target_link_libraries(head_model PRIVATE OpenMP::OpenMP_CXX)
endif()

target_include_directories(head_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(head_model PUBLIC cxx_std_17)
