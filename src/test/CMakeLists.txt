set(TARGETNAME Client)

include_directories(../server_common)

set(SRCS 
	test_client.cpp)

add_executable(${TARGETNAME} ${SRCS})
target_link_libraries(${TARGETNAME} ${ALLLIBS})
install(TARGETS ${TARGETNAME} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

set(TEST_SRCS 
	test.cpp)

add_executable(Test ${TEST_SRCS})
target_link_libraries(Test ${ALLLIBS})
install(TARGETS Test DESTINATION ${EXECUTABLE_OUTPUT_PATH})