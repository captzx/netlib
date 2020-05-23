cmake_minimum_required(VERSION 3.10)
set(TARGETNAME xtools)
project(${TARGETNAME})

add_compile_options(-DBOOST_ALL_DYN_LINK)
set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(HEADERS
	Common.h
	Config.h
	Crypto.h
	DBConnection.h
	Logger.h
	NoCopyable.h
	Singleton.h
	Type.h
	Time.h
	Uuid.h)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${TARGETNAME})

set(SRCS 
	Common.cpp
	Time.cpp
	Logger.cpp
	Crypto.cpp
	Uuid.cpp
	DBConnection.cpp)
add_library(${TARGETNAME} SHARED ${SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
install(TARGETS ${TARGETNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})

