add_executable(example)

target_sources(example
	PRIVATE
		example.c
)

target_link_libraries(example
	PRIVATE
		cava
)
