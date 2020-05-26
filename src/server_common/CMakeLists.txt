cmake_minimum_required(VERSION 3.10)
set(TARGETNAME xcommon)
project(${TARGETNAME})

add_compile_options(-DBOOST_ALL_DYN_LINK)
set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(HEADERS
	UseTools.h
	UseNet.h
	Server.h
	)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${TARGETNAME})

set(INCLUDEDIRS ${HEADER_OUTPUT_PATH}) 
include_directories(${INCLUDEDIRS})
set(SRCS 
    Server.cpp
	)
add_library(${TARGETNAME} ${SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
install(TARGETS ${TARGETNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})