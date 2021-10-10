cmake_minimum_required(VERSION 3.13)
project(DwarvesVsZombies)

if (WIN32)
	add_subdirectory(DVZClient)
endif()
add_subdirectory(DVZCore)
add_subdirectory(DVZServer)