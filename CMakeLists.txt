cmake_minimum_required(VERSION 3.10)

set(PROJECTNAME xserver)

project(${PROJECTNAME})

add_compile_options(-std=c++1y 
	-DBOOST_ALL_DYN_LINK
	-llib_protobuf
	-lz
	-lxnet 
	-lxtools)

#include_directories(${PROJECT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(PkgConfig)
pkg_search_module(PROTOBUF REQUIRED protobuf)

set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

MESSAGE(STATUS ${PROTOBUF_LIBRARIES})
MESSAGE(STATUS ${PROTOBUF_INCLUDE_DIRS})

set(INCLUDEDIRS 
	./src/common
	/usr/local/include 
	${PROJECT_SOURCE_DIR}/include
	./src/protos
	${PROTOBUF_INCLUDE_DIRS}
	) 

set(LIBRARYDIRS 
	/usr/local/lib
	/usr/local/lib64
	${PROTOBUF_LIBRARIES}
	${PROJECT_SOURCE_DIR}/libs
	)

include_directories(${INCLUDEDIRS})
link_directories(${LIBRARYDIRS})

set(BOOST_LIBS 
	libboost_log.so 
	libboost_log_setup.so 
	libboost_thread.so
	libboost_locale.so)

set(LINUX_LIBS
	libpthread.so
	libz.so)

set(XML_LIBS
	libtinyxml2.so)
	
set(PROTOBUF_LIBS
	libprotobuf.so)
	
set(X_LIBS 
	libxnet.so
	libxtools.so
	libxprotos.so)

set(ALLLIBS
	${LINUX_LIBS}
	${BOOST_LIBS}
	${XML_LIBS}
	${PROTOBUF_LIBS}
	${X_LIBS})

add_subdirectory(src)
