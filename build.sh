#!/usr/bin/env bash

for arg; do declare $arg='1'; done
if [[ -v cross ]]; then cross=1; fi
if [[ ! -v release ]]; then debug=1; fi
if [[ -v debug ]]; then echo "debug mode"; fi
if [[ -v release ]]; then echo "release mode"; fi

compiler='gcc'
archiver='ar'
platform_src=''
platform_obj=''
linker_flags=''
compiler_flags='-std=gnu23 -Wfatal-errors'

if [[ -v cross ]]; then
	compiler='x86_64-w64-mingw32-gcc'
	archiver='x86_64-w64-mingw32-ar'
	platform_src+='src/windows/graphics_d2d.c src/windows/graphics_d3d11.c src/windows/graphics.c src/windows/window.c'
	platform_obj+='graphics_d2d.o graphics_d3d11.o graphics.o'
	linker_flags+='-ld2d1 -ld3d11 -lole32 -luuid'
fi

if [[ -v debug ]]; then
	compiler_flags+=" -Wall -Wextra -Wpedantic -Werror -g -O0"
fi

echo "compiling static lib"
if ! ${compiler} ${flags} -Iinclude -c src/cava.c ${platform_src}; then
	echo "compilation failed"
	exit 1
fi
if ! ${archiver} rcs libcava.a cava.o window.o ${platform_obj}; then
	echo "archiving failed"
	exit 1
fi

rm cava.o
rm window.o
rm ${platform_obj}

mkdir -p lib
mv libcava.a lib

compile="${compiler} ${compiler_flags} -fsyntax-only -Iinclude -Llib example/example.c -lcava ${linker_flags}"

if ! bear -o build/compile_commands.json -- $compile; then
	echo "failed to generate compile_commands"
	exit 1
fi

if [[ -v debug ]]; then
	echo "compiling example program"
	compile="${compiler} ${compiler_flags} -Iinclude -Llib example/example.c -o example.exe -lcava ${linker_flags}"

	$compile
fi

echo "success!"
