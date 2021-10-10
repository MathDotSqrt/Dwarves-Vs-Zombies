
set(HEADER_FILES 
	"./include/core/common.hpp"
	"./include/core/CoreComponents.hpp"

	"./include/core/collision/AABB.hpp"

	"./include/core/voxel/block.hpp"
	"./include/core/voxel/chunk.hpp"
	"./include/core/voxel/VoxelCollision.hpp"
)

set(SOURCE_FILES 
	"./src/collision/AABB.cpp"

	"./src/voxel/block.cpp"
	"./src/voxel/chunk.cpp"
	"./src/voxel/VoxelCollision.cpp"
)

add_library(DVZCore ${HEADER_FILES} ${SOURCE_FILES})
target_compile_features(DVZCore PUBLIC cxx_std_17)

find_package(glm REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

find_package(GameNetworkingSockets CONFIG REQUIRED)

target_include_directories(DVZCore PUBLIC "./include") 
target_link_libraries(DVZCore PUBLIC 
	glm::glm
	EnTT::EnTT
	spdlog 
	GameNetworkingSockets::shared
)