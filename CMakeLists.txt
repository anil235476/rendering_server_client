cmake_minimum_required(VERSION 3.7)

set(LIBWEBRTC_INCLUDE_PATH "D:/webrtcBuild/Sept/webrtc_checkout/src")
set(ABSL_PATH "${LIBWEBRTC_INCLUDE_PATH}/third_party/abseil-cpp")
set(BOOST_LIB "D:/boost_1_69_0/boost_1_69_0")

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
	$<BUILD_INTERFACE:${BOOST_LIB}>
	$<BUILD_INTERFACE:${ABSL_PATH}>
	$<BUILD_INTERFACE:${SIGNALLER_LIB_PATH}>
	$<BUILD_INTERFACE:${JSON_INCLUDE}>
	$<BUILD_INTERFACE:${JSON_LIB_PATH}>
	)
