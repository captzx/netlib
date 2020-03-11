cmake_minimum_required(VERSION 3.10)

project(netlib)

add_compile_options(-std=c++1y -g -DBOOST_ALL_DYN_LINK)

include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/include)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(src)
