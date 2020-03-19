set(SRCS 
	Logger.cpp)

add_library(pursue_tools ${SRCS})
target_link_libraries(pursue_tools ${BOOST_LIBS})

install(TARGETS pursue_tools DESTINATION ${LIBRARY_OUTPUT_PATH})

set(HEADERS 
	Logger.h)
	
install(FILES ${HEADERS} DESTINATION ${HEADER_OUTPUT_PATH}/pursue_tools)

