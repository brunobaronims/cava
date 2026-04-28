#pragma once

#include <windows.h>

typedef struct Window {
	HWND hwnd;
	HINSTANCE hinstance;
} Window;

int window_init(Window *window);
void window_deinit(Window *window);
void window_run();

enum {
	ERR_COULD_NOT_SET_AWARENESS = 1,
	ERR_NULL_WINDOW,
	ERR_COULD_NOT_GET_HANDLE,
	ERR_COULD_NOT_CREATE_WINDOW,
};
