set(SRCS 
	LoginConfig.cpp
	LoginServer.cpp
	main.cpp)

add_executable(logonserver ${SRCS})

target_link_libraries(logonserver ${ALLLIBS})

install(TARGETS logonserver DESTINATION ${EXECUTABLE_OUTPUT_PATH})