#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windowsx.h>
#include "graphics.h"

typedef struct {
	CavaGraphics graphics;
	HWND hwnd;
	HINSTANCE hinstance;
	UINT dpi_scale;
} CavaWindow;

int cava_window_init(CavaWindow *window);
void cava_window_deinit(CavaWindow *window);

void cava_run(CavaWindow *window);

enum {
	ERR_COULD_NOT_SET_AWARENESS = 1,
	ERR_NULL_WINDOW,
	ERR_COULD_NOT_GET_HANDLE,
	ERR_COULD_NOT_CREATE_WINDOW,
	ERR_COULD_NOT_CREATE_FACTORY,
	ERR_COULD_NOT_INITIALIZE_COM,
	ERR_COULD_NOT_CREATE_RENDER_TARGET,
};
