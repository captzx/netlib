cmake_minimum_required(VERSION 3.10)
set(TARGETNAME xnet)
project(${TARGETNAME})

add_compile_options(-DBOOST_ALL_DYN_LINK)
set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(HEADERS
	Using.h
	Buffer.h
	TcpService.h
	ProtobufProcess.h
	)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${TARGETNAME})

set(INCLUDEDIRS ${HEADER_OUTPUT_PATH}) 
include_directories(${INCLUDEDIRS})
set(SRCS 
    Buffer.cpp
	TcpService.cpp
	ProtobufProcess.cpp
	)
add_library(${TARGETNAME} SHARED ${SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
install(TARGETS ${TARGETNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})