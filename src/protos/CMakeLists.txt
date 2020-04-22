cmake_minimum_required(VERSION 3.10)

set(PROJECTNAME xprotos)

project(${PROJECTNAME})

add_compile_options(-std=c++1y -llib_protobuf)

set(SRCS 
	login.pb.cc)

add_library(${PROJECTNAME} SHARED ${SRCS})

set(PROTOBUF_LIBS libprotobuf.so)

target_link_libraries(${PROJECTNAME} ${PROTOBUF_LIBS})

set(HEADERS
	login.pb.h)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)

install(TARGETS ${PROJECTNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${PROJECTNAME})

