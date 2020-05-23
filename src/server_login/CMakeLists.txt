set(TARGETNAME LoginServer)

include_directories(../server_common)

set(SRCS
	LoginServer.cpp
	LoginUser.cpp
	LoginUserManager.cpp
	main.cpp)

add_executable(${TARGETNAME} ${SRCS})
target_link_libraries(${TARGETNAME} ${ALLLIBS})
install(TARGETS ${TARGETNAME} DESTINATION ${EXECUTABLE_OUTPUT_PATH})