cmake_minimum_required(VERSION 3.10)

set(PROJECTNAME xtools)

project(${PROJECTNAME})

add_compile_options(-std=c++1y 
	-DBOOST_ALL_DYN_LINK 
	-llib_tinyxml2)

set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb3 -s")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(SRCS 
	Logger.cpp)

add_library(${PROJECTNAME} SHARED ${SRCS})

target_link_libraries(${PROJECTNAME} 
	${BOOST_LIBS}
	libtinyxml2.so)

set(HEADERS
	Type.h
	Config.h
	Logger.h
	Singleton.h)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../libs)
set(HEADER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../include)

install(TARGETS ${PROJECTNAME} DESTINATION ${LIBRARY_OUTPUT_PATH})
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/${PROJECTNAME})
