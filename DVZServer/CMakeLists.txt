set(HEADER_FILES
	"./include/server/Engine.hpp"
	"./include/server/ServerNetRecvSystem.hpp"
	"./include/server/ServerNetSendSystem.hpp"
	"./include/server/PhysicsSystem.hpp"
	"./include/server/ServerComponents.hpp"
	"./include/server/ServerVoxelSystem.hpp"

	"./include/server/net/EntitySnapshot.hpp"
	"./include/server/net/NetServerManager.hpp"
	"./include/server/net/ServerSocket.hpp"

	"./include/server/voxel/ServerChunkManager.hpp"
)

set(SOURCE_FILES 
	"./src/main.cpp"

	"./src/Engine.cpp"
	"./src/ServerNetRecvSystem.cpp"
	"./src/ServerNetSendSystem.cpp"
	"./src/PhysicsSystem.cpp"
	"./src/ServerVoxelSystem.cpp"

	"./src/net/EntitySnapshot.cpp"
	"./src/net/NetServerManager.cpp"
	"./src/net/ServerSocket.cpp"

	"./src/voxel/ServerChunkManager.cpp"
)

add_executable(DVZServer ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(DVZServer PRIVATE "./include")
target_link_libraries(DVZServer PRIVATE DVZCore)