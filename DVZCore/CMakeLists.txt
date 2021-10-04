
set(HEADER_FILES 
	"./include/core/common.hpp"
	
	"./include/core/voxel/block.hpp"
	"./include/core/voxel/chunk.hpp"
)

set(SOURCE_FILES 
	"./src/voxel/block.cpp"
	"./src/voxel/chunk.cpp"
)

add_library(DVZCore ${HEADER_FILES} ${SOURCE_FILES})
target_compile_features(DVZCore PUBLIC cxx_std_17)

find_package(glm REQUIRED)


target_include_directories(DVZCore PUBLIC "./include") 
target_link_libraries(DVZCore PUBLIC 
	glm::glm 
)