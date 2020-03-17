# set(src test_server.cpp ../common/Logger.cpp)

add_executable(test_server test_server.cpp ../common/Logger.cpp)
target_link_libraries(test_server libboost_log.so libboost_log_setup.so libboost_thread.so libpthread.so libboost_locale.so)

add_executable(test_client test_client.cpp ../common/Logger.cpp)
target_link_libraries(test_client libboost_log.so libboost_log_setup.so libboost_thread.so libpthread.so libboost_locale.so)