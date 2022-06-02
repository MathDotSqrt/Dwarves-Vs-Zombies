
set(HEADER_FILES 
	"./include/core/common.hpp"
	"./include/core/CoreComponents.hpp"
	"./include/core/MovementUtil.hpp"
	"./include/core/time.hpp"

	"./include/core/collision/AABB.hpp"

	"./include/core/net/ConnectionState.hpp"
	"./include/core/net/EntitySnapshotDelta.hpp"
	"./include/core/net/Packet.hpp"

	"./include/core/util/CircularBuffer.hpp"
	"./include/core/util/Frustum.hpp"
	"./include/core/util/math.hpp"
	"./include/core/util/PoolAllocator.hpp"
	"./include/core/util/serialization.hpp"
	"./include/core/util/SpiralIter.hpp"
	"./include/core/util/Timer.hpp"
	"./include/core/util/transform.hpp"
	"./include/core/util/util.hpp"
	"./include/core/util/view_streambuf.hpp"


	"./include/core/voxel/block.hpp"
	"./include/core/voxel/chunk.hpp"
	"./include/core/voxel/ChunkData.hpp"
	"./include/core/voxel/IChunk.hpp"
	"./include/core/voxel/IVoxelWorld.hpp"
	"./include/core/voxel/VoxelConstants.hpp"
	"./include/core/voxel/VoxelCollision.hpp"
)

set(SOURCE_FILES 
	"./src/MovementUtil.cpp"

	"./src/collision/AABB.cpp"

	"./src/net/EntitySnapshotDelta.cpp"
	"./src/net/Packet.cpp"

	"./src/util/Frustum.cpp"
	"./src/util/Timer.cpp"
	"./src/util/transform.cpp"

	"./src/voxel/block.cpp"
	"./src/voxel/chunk.cpp"
	"./src/voxel/ChunkData.cpp"
	"./src/voxel/IChunk.cpp"
	"./src/voxel/IVoxelWorld.cpp"
	"./src/voxel/VoxelConstants.cpp"
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
	spdlog::spdlog spdlog::spdlog_header_only
	GameNetworkingSockets::shared
)
