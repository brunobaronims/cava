#pragma once
#if defined(_WIN32)
#include "windows/window.h"
#endif

typedef struct {
	CavaWindow window;
} Cava;

int cava_init(Cava *cava);
void cava_deinit(Cava *cava);
