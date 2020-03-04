set(src test_glog.cpp)

add_executable(test_glog ${src})
target_link_libraries(test_glog libglog.so)
