set(src test_glog.cpp)

add_executable(test_glog ${src})
target_link_libraries(test_glog libboost_log.so libboost_log_setup.so libboost_thread.so libpthread.so libboost_locale.so)
