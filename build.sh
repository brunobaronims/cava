#!/usr/bin/env bash

for arg; do declare $arg='1'; done
if [[ -v cross ]]; then cross=1; fi
if [[ ! -v release ]]; then debug=1; fi
if [[ -v debug ]]; then echo "debug mode"; fi
if [[ -v release ]]; then echo "release mode"; fi

if [[ ! -v cross ]]; then
	echo "only cross compilation with mingw-w64-gcc is currently supported"
	exit 1
fi

compiler='gcc'
archiver='ar'
platform_src=''
platform_obj=''
linker_flags=''

if [[ -v cross ]]; then
	compiler='x86_64-w64-mingw32-gcc'
	archiver='x86_64-w64-mingw32-ar'
	platform_src+='src/windows/direct2d.c src/windows/window.c'
	platform_obj+='direct2d.o'
	linker_flags+='-ld2d1 -lole32 -luuid'
fi

flags=''
if [[ -v debug ]]; then
	flags+="-Wall -Wextra -Wpedantic -Werror -g -O0"
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

if [[ -v debug ]]; then
	echo "compiling example program"

	if ! ${compiler} ${flags} -Iinclude -Llib example/example.c -o example.exe -lcava ${linker_flags}; then
		echo "example compilation failed"
		exit 1
	fi
fi

echo "success!"
