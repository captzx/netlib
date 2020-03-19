# set(src test_server.cpp ../common/Logger.cpp)

add_executable(test_server test_server.cpp)
target_link_libraries(test_server 
	${BOOST_LIBS}
	libpursue_netserver.a
	libpursue_tools.a)

add_executable(test_client test_client.cpp)
target_link_libraries(test_client 
	${BOOST_LIBS}
	libpursue_netserver.a
	libpursue_tools.a)

install(TARGETS test_server DESTINATION ${EXECUTABLE_OUTPUT_PATH})
install(TARGETS test_client DESTINATION ${EXECUTABLE_OUTPUT_PATH})