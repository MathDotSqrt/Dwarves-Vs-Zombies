set(HEADER_FILES
	"./include/server/Engine.hpp"
	"./include/server/NetSystem.hpp"
	"./include/server/ServerComponents.hpp"

	"./include/server/net/NetServerManager.hpp"
	"./include/server/net/ServerSocket.hpp"
)

set(SOURCE_FILES 
	"./src/main.cpp"

	"./src/Engine.cpp"
	"./src/NetSystem.cpp"

	"./src/net/NetServerManager.cpp"
	"./src/net/ServerSocket.cpp"
)

add_executable(DVZServer ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(DVZServer PRIVATE "./include")
target_link_libraries(DVZServer PRIVATE DVZCore)