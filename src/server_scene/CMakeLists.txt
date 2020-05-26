set(TARGETNAME SceneServer)

set(SRCS
	SceneServer.cpp
	)

add_executable(${TARGETNAME} ${SRCS})
target_link_libraries(${TARGETNAME} ${ALLLIBS})
install(TARGETS ${TARGETNAME} DESTINATION ${EXECUTABLE_OUTPUT_PATH})