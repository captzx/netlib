cmake_minimum_required(VERSION 3.10)

set(PROJECTNAME xserver)

project(${PROJECTNAME})

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

add_compile_options(-std=c++17
	-DBOOST_ALL_DYN_LINK
	-lz)
set(CMAKE_BUILD_TYPE "debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(INCLUDEDIRS /usr/local/include ${PROJECT_SOURCE_DIR}/include) 
include_directories(${INCLUDEDIRS})

set(LIBRARYDIRS /usr/local/lib /usr/local/lib64 ${PROJECT_SOURCE_DIR}/libs)
link_directories(${LIBRARYDIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LINUX_LIBS libpthread.so libz.so)
set(BOOST_LIBS libboost_log.so libboost_log_setup.so libboost_thread.so libboost_locale.so)
set(XML_LIBS libtinyxml2.so)
set(PROTOBUF_LIBS libprotobuf.so)
set(CRYPTO_LIBS libcryptopp.so)
set(MYSQL_LIBS libmysqlcppconn8.so)
set(X_LIBS libxnet.so libxtools.so libxprotos.so)
set(ALLLIBS ${LINUX_LIBS} ${BOOST_LIBS} ${XML_LIBS} ${PROTOBUF_LIBS}  ${CRYPTO_LIBS} ${MYSQL_LIBS} ${X_LIBS})

add_subdirectory(src)
