#pragma once
#if defined(_WIN32)
#include "windows/window.h"
#include <windows.h>
#endif

typedef struct Cava {
	Window window;
} Cava;

int cava_init(Cava *cava);
void cava_deinit(Cava *cava);
void cava_run(void);
