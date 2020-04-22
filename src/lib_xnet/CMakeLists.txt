cmake_minimum_required(VERSION 3.10)

set(PROJECTNAME xnet)

project(${PROJECTNAME})

add_compile_options(-std=c++1y
	-DBOOST_ALL_DYN_LINK 
	-llib_protobuf
	-lz)

set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -s")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(SRCS 
    Buffer.cpp
	NetServer.cpp
	ProtobufCodec.cpp
	TcpConnection.cpp)

add_library(${PROJECTNAME} SHARED ${SRCS})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)

find_package(PkgConfig)
pkg_search_module(PROTOBUF REQUIRED protobuf)

set(INCLUDEDIRS 
	/usr/local/include
	${HEADER_OUTPUT_PATH}
	${PROTOBUF_INCLUDE_DIRS}) 

set(LIBRARYDIRS 
	./src/common
	/usr/local/lib
	/usr/local/lib6
	${LIBRARY_OUTPUT_PATH}
	${PROTOBUF_LIBRARIES})

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
	
set(PROTOBUF_LIBS
	libprotobuf.so)
		
set(X_LIBS
	libxtools.so)

target_link_libraries(${PROJECTNAME} 
	${LINUX_LIBS}
	${BOOST_LIBS}
	${PROTOBUF_LIBS})

set(HEADERS
	Buffer.h
	NetServer.h
	ProtobufCodec.h
	TcpConnection.h)

install(TARGETS ${PROJECTNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})

install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${PROJECTNAME})