set(SRCS 

	../lib_pursue_tools/Logger.cpp
	../lib_pursue_netserver/NetServer.cpp
	../lib_pursue_netserver/TcpConnection.cpp

	LoginConfig.cpp
	LoginServer.cpp
	main.cpp)

add_executable(logonserver ${SRCS})
target_link_libraries(logonserver 
	${BOOST_LIBS})

install(TARGETS logonserver DESTINATION ${EXECUTABLE_OUTPUT_PATH})