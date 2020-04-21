# set(src test_server.cpp ../common/Logger.cpp)

# add_executable(test_server 
# 	../lib_pursue_tools/Logger.cpp
# 	test_server.cpp)
# target_link_libraries(test_server 
# 	${BOOST_LIBS})
set(PROTOSRCS 
	../protos/cpp/login.pb.cc
)

set(LIBPURSUENETSERVERSRCS 
	../lib_pursue_netserver/NetServer.cpp
	../lib_pursue_netserver/TcpConnection.cpp
	../lib_pursue_netserver/Buffer.cpp
	../lib_pursue_netserver/ProtobufCodec.cpp
)
add_executable(test_client 
	../lib_pursue_tools/Logger.cpp
	test_client.cpp
	
	${LIBPURSUENETSERVERSRCS}
	${PROTOSRCS})
target_link_libraries(test_client 
	${BOOST_LIBS})

# install(TARGETS test_server DESTINATION ${EXECUTABLE_OUTPUT_PATH})
install(TARGETS test_client DESTINATION ${EXECUTABLE_OUTPUT_PATH})