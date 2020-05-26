set(TARGETNAME LoginServer)

include_directories(../server_common)

set(SRCS
	LoginServer.cpp
	LoginUserManager.cpp
	ZoneServerManager.cpp)

add_executable(${TARGETNAME} ${SRCS})
target_link_libraries(${TARGETNAME} ${ALLLIBS})
install(TARGETS ${TARGETNAME} DESTINATION ${EXECUTABLE_OUTPUT_PATH})