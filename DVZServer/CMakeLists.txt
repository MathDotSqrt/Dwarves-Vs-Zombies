
set(SOURCE_FILES 
	"./src/main.cpp"
)

add_executable(DVZServer ${SOURCE_FILES})

target_include_directories(DVZServer PRIVATE "./include")
target_link_libraries(DVZServer LINK_PUBLIC DVZCore)