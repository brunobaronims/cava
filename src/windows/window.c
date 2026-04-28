#include "windows/window.h"
#include <windows.h>

static LRESULT CALLBACK window_proc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
);
static HWND new_hwnd(HINSTANCE hinstance, Window *window);

void window_run()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int window_init(Window *window)
{
	if (!window)
		return ERR_NULL_WINDOW;

	window->hwnd = NULL;
	window->hinstance = NULL;

	if (!SUCCEEDED(SetProcessDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
	    )))
		return ERR_COULD_NOT_SET_AWARENESS;

	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	HMODULE hmodule = GetModuleHandle(NULL);
	if (!hmodule)
		return ERR_COULD_NOT_GET_HANDLE;

	HINSTANCE hinstance = (HINSTANCE)hmodule;
	window->hinstance = hinstance;

	HWND hwnd = new_hwnd(hinstance, window);
	if (!hwnd)
		return ERR_COULD_NOT_CREATE_WINDOW;

	window->hwnd = hwnd;

	ShowWindow(window->hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(window->hwnd);

	return 0;
}

void window_deinit(Window *window)
{
	if (!window)
		return;

	if (window->hwnd)
		DestroyWindow(window->hwnd);
}

static HWND new_hwnd(HINSTANCE hinstance, Window *window)
{
	const char *class_name = "ezel";
	const char *window_title = "ezel";
	WNDCLASSEX wc = {0};
	HWND hwnd;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_proc;
	wc.hInstance = hinstance;
	wc.lpszClassName = class_name;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wc);

	hwnd = CreateWindowEx(
	    0,
	    class_name,
	    window_title,
	    WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    CW_USEDEFAULT,
	    NULL,
	    NULL,
	    hinstance,
	    window
	);
	if (!hwnd)
		return NULL;

	return hwnd;
}

static LRESULT CALLBACK window_proc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
)
{
	if (msg == WM_CREATE) {
		CREATESTRUCT *pcs = (CREATESTRUCT *)lparam;
		Window *window = (Window *)pcs->lpCreateParams;
		LONG_PTR result;

		SetLastError(0);
		result = SetWindowLongPtr(
		    hwnd,
		    GWLP_USERDATA,
		    (LONG_PTR)window
		);
		if (result == 0 && GetLastError() != 0)
			return -1;

		return 0;
	}

	LONG_PTR p = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Window *window = p ? (Window *)p : NULL;
	int was_handled = 0;

	if (window) {
		switch (msg) {
		case WM_PAINT:
			ValidateRect(hwnd, NULL);
			was_handled = 1;
			return 0;

		case WM_SIZE:
			was_handled = 1;
			return 0;

		case WM_DISPLAYCHANGE:
			InvalidateRect(hwnd, NULL, 0);
			was_handled = 1;
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			was_handled = 1;
			return 0;
		}
	}

	if (!was_handled)
		return DefWindowProc(hwnd, msg, wparam, lparam);

	return 0;
}
