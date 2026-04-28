cmake_minimum_required(VERSION 4.2.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cava
	VERSION 0.0.1
	LANGUAGES C
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/cavaConfigVersion.cmake
	COMPATIBILITY ExactVersion
)

option(DEBUG "Build debug executable" OFF)

include(GNUInstallDirs)

if(DEBUG) 
	add_subdirectory(example)
	install(
		TARGETS example
	)
endif()

add_subdirectory(include)

install(
	TARGETS cava
	EXPORT cavaTargets
	FILE_SET HEADERS
)

install(
	EXPORT cavaTargets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cava
)

install(
	FILES
		cmake/cavaConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/cavaConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cava
)
