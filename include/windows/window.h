#pragma once

#include <windows.h>
#include <d2d1.h>

typedef struct Window {
	HWND hwnd;
	HINSTANCE hinstance;
	ID2D1Factory *factory;
	ID2D1HwndRenderTarget *render_target;
	ID2D1SolidColorBrush *brush;
	UINT dpi_scale;
} Window;

int window_init(Window *window);
void window_deinit(Window *window);
void window_run(void);

enum {
	ERR_COULD_NOT_SET_AWARENESS = 1,
	ERR_NULL_WINDOW,
	ERR_COULD_NOT_GET_HANDLE,
	ERR_COULD_NOT_CREATE_WINDOW,
	ERR_COULD_NOT_CREATE_FACTORY,
	ERR_COULD_NOT_INITIALIZE_COM,
	ERR_COULD_NOT_CREATE_RENDER_TARGET,
};
