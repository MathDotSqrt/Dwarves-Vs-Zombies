
set(HEADER_FILES 
	"./include/client/engine.hpp"
	"./include/client/window.hpp"

	"./include/client/graphics/Attrib.hpp"
	"./include/client/graphics/BasicRenderer.hpp"
	"./include/client/graphics/GLSLProgram.hpp"
	"./include/client/graphics/Geometry.hpp"
	"./include/client/graphics/GeometryBuilder.hpp"
	"./include/client/graphics/Mesh.hpp"
	"./include/client/graphics/MeshCache.hpp"
	"./include/client/graphics/ShaderSet.hpp"
	"./include/client/graphics/TEX.hpp"
	"./include/client/graphics/TEXCache.hpp"
	"./include/client/graphics/VAO.hpp"
	"./include/client/graphics/VBO.hpp"
)

set(SOURCE_FILES 
	"./src/engine.cpp"
	"./src/main.cpp"
	"./src/window.cpp"

	"./src/graphics/Attrib.cpp"
	"./src/graphics/BasicRenderer.cpp"
	"./src/graphics/GeometryBuilder.cpp"
	"./src/graphics/GLSLProgram.cpp"
	"./src/graphics/ShaderSet.cpp"
	"./src/graphics/TEX.cpp"
	"./src/graphics/VAO.cpp"
	"./src/graphics/VBO.cpp"
)

add_executable(DVZClient ${HEADER_FILES} ${SOURCE_FILES})
target_compile_features(DVZClient PUBLIC cxx_std_17)
target_compile_definitions(DVZClient PRIVATE 
	RES_PATH="${PROJECT_SOURCE_DIR}/DVZClient/res/"
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
target_compile_definitions(DVZClient PRIVATE 
	DVZ_CLIENT_DEBUG
)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
target_compile_definitions(DVZClient PRIVATE 
	DVZ_CLIENT_RELDEBINFO
)
else()
target_compile_definitions(DVZClient PRIVATE 
	DVZ_CLIENT_RELEASE
)
endif()

find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h" REQUIRED)

target_include_directories(DVZClient PRIVATE 
	"./include"
	"./res/shaders/include"
	${STB_INCLUDE_DIRS}
)

target_link_libraries(DVZClient PRIVATE 
	DVZCore 
	glfw
	GLEW::glew
	EnTT::EnTT
	spdlog::spdlog spdlog::spdlog_header_only
)

# Create symlink
function(create_symlink src dest)
	message("Creating Symlink: ${src} ${dest}")

	if (UNIX)
		execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink "${src}" "${dest}")
	else()
		file(TO_NATIVE_PATH "${dest}" _dstDir)
		file(TO_NATIVE_PATH "${src}" _srcDir)
		execute_process(COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
	endif()
endfunction(create_symlink)

create_symlink("${CMAKE_CURRENT_SOURCE_DIR}/res/" "${CMAKE_CURRENT_BINARY_DIR}/res/")