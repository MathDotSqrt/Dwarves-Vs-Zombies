
set(SOURCE_FILES 
	"./src/main.cpp"
)

add_executable(DVZClient ${SOURCE_FILES})

target_include_directories(DVZClient PRIVATE "./include")
target_link_libraries(DVZClient LINK_PUBLIC DVZCore)