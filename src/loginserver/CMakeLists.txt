set(PROTOSRCS 
	../protos/cpp/login.pb.cc
)
set(LIBPURSUETOOLSSRCS 
	../lib_pursue_tools/Logger.cpp
)
set(LIBPURSUENETSERVERSRCS 
	../lib_pursue_netserver/NetServer.cpp
	../lib_pursue_netserver/TcpConnection.cpp
	../lib_pursue_netserver/Buffer.cpp
	../lib_pursue_netserver/ProtobufCodec.cpp
)
set(SRCS 
	LoginConfig.cpp
	LoginServer.cpp
	main.cpp

	${LIBPURSUETOOLSSRCS}
	${LIBPURSUENETSERVERSRCS}
	${PROTOSRCS})

add_executable(logonserver ${SRCS})
target_link_libraries(logonserver 
	${BOOST_LIBS})

install(TARGETS logonserver DESTINATION ${EXECUTABLE_OUTPUT_PATH})