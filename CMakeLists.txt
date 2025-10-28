cmake_minimum_required(VERSION 3.20)
project(primary_fluence LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(HDF5 REQUIRED COMPONENTS C CXX)
find_package(HighFive CONFIG REQUIRED)

add_executable(primary_fluence
  src/main.cpp
  src/h5_model.cpp
  src/aperture.cpp
  src/gaussian.cpp
  src/convolve.cpp
)
target_include_directories(primary_fluence PRIVATE include)
target_link_libraries(primary_fluence PRIVATE HighFive HDF5::HDF5)

if (MSVC)
  target_compile_options(primary_fluence PRIVATE /W4 /permissive-)
else()
  target_compile_options(primary_fluence PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(primary_fluence PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

