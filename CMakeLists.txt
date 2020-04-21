cmake_minimum_required(VERSION 3.10)

project(netlib)

add_compile_options(-std=c++1y 
-DBOOST_ALL_DYN_LINK 
-llib_pursue_netserver 
-llib_pursue_tools 
-llib_tinyxml2 
-llib_protobuf
-lz)

include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/include)

find_package(PkgConfig)
pkg_search_module(PROTOBUF REQUIRED protobuf)

set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

MESSAGE(STATUS ${PROTOBUF_LIBRARIES})
MESSAGE(STATUS ${PROTOBUF_INCLUDE_DIRS})

set(INCLUDEDIRS 
	/usr/local/include 
	./src/common
	./src/lib_pursue_netserver
	./src/lib_pursue_tools
	./src/protos/cpp
	${PROTOBUF_INCLUDE_DIRS}) 

set(LIBRARYDIRS 
	/usr/local/lib
	/usr/local/lib64
	${PROTOBUF_LIBRARIES})

include_directories(${INCLUDEDIRS})
link_directories(${LIBRARYDIRS})

set(BOOST_LIBS 
	libboost_log.so 
	libboost_log_setup.so 
	libboost_thread.so 
	libpthread.so 
	libboost_locale.so
	
	libtinyxml2.so
	libprotobuf.so
	libz.so)

add_subdirectory(src)
