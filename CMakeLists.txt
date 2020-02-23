cmake_minimum_required(VERSION 3.7)


add_library(rendering_server_client
	rendering_server_client.h
	rendering_server_client.cpp
	)

	# Add some compile flags to our source files.
if(MSVC)
	#set_source_files_properties(${SOURCE_FILES}
	#	PROPERTIES COMPILE_FLAGS "/W3 /WX")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

endif()

target_include_directories(rendering_server_client PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)

	target_link_libraries(rendering_server_client PUBLIC 
	    json_parser 
		func_thread_handler 
		signaller)
