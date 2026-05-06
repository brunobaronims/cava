#include "windows/window.h"
#include "windows/direct2d.h"
#include <d2d1.h>
#include <windows.h>

static LRESULT CALLBACK window_proc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam
);
static HWND new_hwnd(HINSTANCE hinstance, Window *window);
static HRESULT on_render(Window *window);

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
	window->render_target = NULL;
	window->brush = NULL;
	window->factory = NULL;

	if (!SUCCEEDED(CoInitialize(NULL))) {
		return ERR_COULD_NOT_INITIALIZE_COM;
	}

	HRESULT hr = create_device_independent_resources(&window->factory);
	if (!SUCCEEDED(hr))
		return ERR_COULD_NOT_CREATE_FACTORY;

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

	direct2d_deinit(window);

	CoUninitialize();
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
			on_render(window);
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
			return 1;
		}
	}

	if (!was_handled)
		return DefWindowProc(hwnd, msg, wparam, lparam);

	return 0;
}

static HRESULT on_render(Window *window)
{
	HRESULT hr = S_OK;

	hr = create_device_resources(window);
	if (!SUCCEEDED(hr))
		return hr;

	RECT rc;
	if (!GetClientRect(window->hwnd, &rc))
		return HRESULT_FROM_WIN32(GetLastError());

	FLOAT dpi_x = 96.0f;
	FLOAT dpi_y = 96.0f;
	ID2D1HwndRenderTarget_GetDpi(window->render_target, &dpi_x, &dpi_y);

	D2D1_SIZE_F size = {
	    .width = (FLOAT)(rc.right - rc.left) * 96.0f / dpi_x,
	    .height = (FLOAT)(rc.bottom - rc.top) * 96.0f / dpi_y,
	};

	ID2D1HwndRenderTarget_BeginDraw(window->render_target);

	D2D1_MATRIX_3X2_F transform = {
		._11 = 1.0f,
		._22 = 1.0f,
	};
	ID2D1HwndRenderTarget_SetTransform(window->render_target, &transform);

	D2D1_COLOR_F white = {
	    .r = 1.0f,
	    .b = 1.0f,
	    .g = 1.0f,
	    .a = 1.0f,
	};
	ID2D1HwndRenderTarget_Clear(window->render_target, &white);

	D2D1_RECT_F rectangle = {
		.top = size.height / 2 - 50.0f,
		.bottom = size.height / 2 + 50.0f,
		.left = size.width / 2 - 50.0f,
		.right = size.width / 2 + 50.0f,
	};

	ID2D1HwndRenderTarget_FillRectangle(
	    window->render_target,
	    &rectangle,
	    (ID2D1Brush *)window->brush
	);

	hr = ID2D1HwndRenderTarget_EndDraw(window->render_target, NULL, NULL);
	if (hr == D2DERR_RECREATE_TARGET) {
		hr = S_OK;
		discard_device_resources(window);
	}

	return hr;
}
