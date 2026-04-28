add_library(cava)

target_compile_features(cava PRIVATE c_std_23)

target_sources(cava
	PRIVATE
		../src/cava.c
	PUBLIC
		FILE_SET HEADERS
		FILES
			cava.h
)

if (WIN32)
	target_sources(cava
		PRIVATE
			../src/windows/window.c
		PUBLIC 
			FILE_SET HEADERS
			FILES
				windows/window.h
	)	
endif()

if(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
	target_compile_options(cava PRIVATE /W3)
elseif(
	(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU") OR
	(CMAKE_C_COMPILER_FRONTEND_VARIANT MATCHES "Clang")
)
	target_compile_options(cava PRIVATE -Wall)
endif()
