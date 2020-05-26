cmake_minimum_required(VERSION 3.10)
set(TARGETNAME xprotos)
project(${TARGETNAME})

set(HEADERS
	Server.pb.h
	Login.pb.h
	Scene.pb.h)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${TARGETNAME})

set(SRCS 
	Server.pb.cc
	Login.pb.cc
	Scene.pb.cc)
add_library(${TARGETNAME} SHARED ${SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
install(TARGETS ${TARGETNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})

