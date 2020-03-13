cmake_minimum_required(VERSION 3.10)

project(netlib)

add_compile_options(-std=c++1y -DBOOST_ALL_DYN_LINK)

include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(src)
