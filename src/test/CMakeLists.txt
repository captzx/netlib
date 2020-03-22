# set(src test_server.cpp ../common/Logger.cpp)

# add_executable(test_server 
# 	../lib_pursue_tools/Logger.cpp
# 	test_server.cpp)
# target_link_libraries(test_server 
# 	${BOOST_LIBS})

add_executable(test_client 
	../lib_pursue_tools/Logger.cpp
	test_client.cpp)
target_link_libraries(test_client 
	${BOOST_LIBS})

# install(TARGETS test_server DESTINATION ${EXECUTABLE_OUTPUT_PATH})
install(TARGETS test_client DESTINATION ${EXECUTABLE_OUTPUT_PATH})