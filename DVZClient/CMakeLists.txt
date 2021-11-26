
set(HEADER_FILES 
	"./include/client/ClientComponents.hpp"
	"./include/client/engine.hpp"
	"./include/client/predict_client.hpp"
	"./include/client/window.hpp"

	"./include/client/net/ClientSocket.hpp"
	"./include/client/net/NetClientManager.hpp"
	"./include/client/net/NetComponentUtil.hpp"

	"./include/client/graphics/Attrib.hpp"
	"./include/client/graphics/BasicRenderer.hpp"
	"./include/client/graphics/GLSLProgram.hpp"
	"./include/client/graphics/Geometry.hpp"
	"./include/client/graphics/GeometryBuilder.hpp"
	"./include/client/graphics/Mesh.hpp"
	"./include/client/graphics/MeshCache.hpp"
	"./include/client/graphics/Scene.hpp"
	"./include/client/graphics/ShaderSet.hpp"
	"./include/client/graphics/TEX.hpp"
	"./include/client/graphics/TEXCache.hpp"
	"./include/client/graphics/VAO.hpp"
	"./include/client/graphics/VBO.hpp"

	"./include/client/systems/ClientPredictionSystem.hpp"
	"./include/client/systems/System.hpp"
	"./include/client/systems/InputSystem.hpp"
	"./include/client/systems/NetworkSystem.hpp"
	"./include/client/systems/PhysicsSystem.hpp"
	"./include/client/systems/RenderSystem.hpp"
	"./include/client/systems/VoxelSystem.hpp"

	"./include/client/util/camera_util.hpp"
	"./include/client/util/freelist.hpp"
	"./include/client/util/id.hpp"
	"./include/client/util/packedfreelist.hpp"
	"./include/client/util/util.hpp"

	"./include/client/voxel/ChunkGeometry.hpp"
	"./include/client/voxel/ChunkMesher.hpp"
	"./include/client/voxel/ChunkRenderData.hpp"
	"./include/client/voxel/ChunkRenderDataManager.hpp"
	"./include/client/voxel/ClientChunkManager.hpp"

)

set(SOURCE_FILES 
	"./src/ClientComponents.cpp"
	"./src/engine.cpp"
	"./src/predict_client.cpp"
	"./src/main.cpp"
	"./src/window.cpp"

	"./src/net/ClientSocket.cpp"
	"./src/net/NetClientManager.cpp"
	"./src/net/NetComponentUtil.cpp"

	"./src/graphics/Attrib.cpp"
	"./src/graphics/BasicRenderer.cpp"
	"./src/graphics/GeometryBuilder.cpp"
	"./src/graphics/GLSLProgram.cpp"
	"./src/graphics/Scene.cpp"
	"./src/graphics/ShaderSet.cpp"
	"./src/graphics/TEX.cpp"
	"./src/graphics/VAO.cpp"
	"./src/graphics/VBO.cpp"

	"./src/systems/ClientPredictionSystem.cpp"
	"./src/systems/InputSystem.cpp"
	"./src/systems/NetworkSystem.cpp"
	"./src/systems/PhysicsSystem.cpp"
	"./src/systems/RenderSystem.cpp"
	"./src/systems/VoxelSystem.cpp"

	"./src/util/camera_util.cpp"

	"./src/voxel/ChunkGeometry.cpp"
	"./src/voxel/ChunkMesher.cpp"
	"./src/voxel/ChunkRenderData.cpp"
	"./src/voxel/ChunkRenderDataManager.cpp"
	"./src/voxel/ClientChunkManager.cpp"

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