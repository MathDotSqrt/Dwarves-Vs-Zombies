
set(HEADER_FILES 
	"./include/client/window.hpp"
)

set(SOURCE_FILES 
	"./src/main.cpp"
	"./src/window.cpp"
)

add_executable(DVZClient ${HEADER_FILES} ${SOURCE_FILES})

find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)


target_include_directories(DVZClient PRIVATE "./include")
target_link_libraries(DVZClient PRIVATE 
	DVZCore 
	glfw
	GLEW::glew
)