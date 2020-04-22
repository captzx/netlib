add_executable(test_client 
	test_client.cpp
	)
target_link_libraries(test_client ${ALLLIBS})

install(TARGETS test_client DESTINATION ${EXECUTABLE_OUTPUT_PATH})