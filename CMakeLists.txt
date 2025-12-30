cmake_minimum_required(VERSION 3.16)

project(MuMain LANGUAGES CXX)

add_subdirectory("Source Main 5.2")
if(CMAKE_GENERATOR MATCHES "Visual Studio")
	add_subdirectory("ClientLibrary")

	set_property(DIRECTORY ${CMAKE_SOURCE_DIR}
		PROPERTY VS_STARTUP_PROJECT Main
	)
endif()