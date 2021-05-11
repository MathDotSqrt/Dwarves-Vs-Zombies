
set(HEADER_FILES 
	"./include/client/engine.hpp"
	"./include/client/window.hpp"

	"./include/client/graphics/BasicRenderer.hpp"
	"./include/client/graphics/GLSLProgram.hpp"
	"./include/client/graphics/ShaderSet.hpp"
	"./include/client/graphics/TEX.hpp"
	"./include/client/graphics/VAO.hpp"
	"./include/client/graphics/VBO.hpp"

)

set(SOURCE_FILES 
	"./src/engine.cpp"
	"./src/main.cpp"
	"./src/window.cpp"

	"./src/graphics/BasicRenderer.cpp"
	"./src/graphics/GLSLProgram.cpp"
	"./src/graphics/ShaderSet.cpp"
	"./src/graphics/TEX.cpp"
	"./src/graphics/VAO.cpp"
	"./src/graphics/VBO.cpp"
)

add_executable(DVZClient ${HEADER_FILES} ${SOURCE_FILES})

target_compile_definitions(DVZClient PRIVATE RES_PATH="${PROJECT_SOURCE_DIR}/res/")

find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h" REQUIRED)

target_include_directories(DVZClient PRIVATE 
	"./include"
	${STB_INCLUDE_DIRS}
)

target_link_libraries(DVZClient PRIVATE 
	DVZCore 
	glfw
	GLEW::glew
	EnTT::EnTT
	spdlog::spdlog spdlog::spdlog_header_only
)