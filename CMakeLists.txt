cmake_minimum_required(VERSION 3.20)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Source and build directories cannot be the same.")
endif()

project(
   bpbp
   LANGUAGES C CXX
)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-pointer-sign -Wstack-protector")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wstack-protector")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../module)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../module)
set(BUILD_OBJECTS_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/.obj)

if (NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
   message(STATUS "Setting CMAKE_BUILD_TYPE to default Release")
   set(CMAKE_BUILD_TYPE "Release" CACHE INTERNAL "Default build type")
endif()

message(STATUS "Building for: ${CMAKE_BUILD_TYPE}")

find_package(pybind11 CONFIG REQUIRED)

# add_library(${PROJECT_NAME} MODULE kruskal_on_hypergraph.cpp)
# target_link_libraries(${PROJECT_NAME} PUBLIC pybind11::module)

pybind11_add_module(${PROJECT_NAME} MODULE kruskal_on_hypergraph.cpp)
