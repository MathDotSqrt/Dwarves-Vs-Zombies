
set(HEADER_FILES 
	"./include/core/test.hpp"
)

set(SOURCE_FILES 
	"./src/test.cpp"
)

add_library(DVZCore ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(DVZCore PUBLIC "./include")