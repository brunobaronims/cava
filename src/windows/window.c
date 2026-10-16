#include "windows/window.h"
#include <stdio.h>

static LRESULT CALLBACK window_proc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
);
static HWND new_hwnd(HINSTANCE hinstance, CavaWindow *window);

void cava_run(CavaWindow *window)
{
	ShowWindow(window->hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(window->hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int cava_window_init(CavaWindow *window)
{
	if (!window)
		return ERR_NULL_WINDOW;

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
	window->dpi_scale = GetDpiForWindow(window->hwnd) / 96.0f;
	window->graphics = (CavaGraphics){0};

	if (!SUCCEEDED(CoInitialize(NULL))) {
		return ERR_COULD_NOT_INITIALIZE_COM;
	}

	if (cava_graphics_init(&window->graphics))
		return ERR_COULD_NOT_CREATE_FACTORY;

	return 0;
}

void cava_window_deinit(CavaWindow *window)
{
	if (!window)
		return;

	cava_graphics_deinit(&window->graphics);

	CoUninitialize();
}

static HWND new_hwnd(HINSTANCE hinstance, CavaWindow *window)
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
		CavaWindow *window = (CavaWindow *)pcs->lpCreateParams;
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
	CavaWindow *window = p ? (CavaWindow *)p : NULL;
	int was_handled = 0;

	if (window) {
		switch (msg) {
		case WM_PAINT:
			cava_graphics_on_render(&window->graphics, window->hwnd, window->dpi_scale);
			ValidateRect(hwnd, NULL);
			was_handled = 1;
			return 0;

		case WM_SIZE: {
			UINT width = LOWORD(lparam);
			UINT height = HIWORD(lparam);
			cava_graphics_on_resize(
			    &window->graphics,
			    height,
			    width
			);
			was_handled = 1;
			return 0;
		}
		case WM_DISPLAYCHANGE:
			InvalidateRect(hwnd, NULL, 0);
			was_handled = 1;
			return 0;
		case WM_DPICHANGED: {
			window->dpi_scale = LOWORD(wparam) / 96.0f;

			RECT *rect = (RECT *)lparam;
			SetWindowPos(
			    hwnd,
			    NULL,
			    rect->left,
			    rect->top,
			    rect->right - rect->left,
			    rect->bottom - rect->top,
			    SWP_NOZORDER | SWP_NOACTIVATE
			);

			InvalidateRect(hwnd, NULL, FALSE);
			was_handled = 1;
			return 0;
		}
		case WM_MOUSEMOVE: {
			int x = GET_X_LPARAM(lparam) / window->dpi_scale;
			int y = GET_Y_LPARAM(lparam) / window->dpi_scale;

			printf("x: %i\n", x);
			printf("y: %i\n", y);
			was_handled = 1;
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			was_handled = 1;
			return 1;
		}
	}

	if (!was_handled)
		return DefWindowProc(hwnd, msg, wparam, lparam);

	return 0;
}
