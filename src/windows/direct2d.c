#include "windows/direct2d.h"
#include "windows/window.h"
#include <d2d1.h>
#include <windows.h>

static void release(void **obj);

HRESULT create_device_independent_resources(ID2D1Factory **factory)
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(
	    D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    &IID_ID2D1Factory,
	    NULL,
	    (void **)factory
	);

	return hr;
}

HRESULT create_device_resources(Window *window)
{
	HRESULT hr = S_OK;

	if (window->render_target)
		return hr;

	RECT rc;
	if (!GetClientRect(window->hwnd, &rc))
		return HRESULT_FROM_WIN32(GetLastError());

	D2D1_SIZE_U size = {
	    .width = rc.right - rc.left,
	    .height = rc.bottom - rc.top,
	};

	D2D1_RENDER_TARGET_PROPERTIES render_target_properties = {0};
	D2D1_HWND_RENDER_TARGET_PROPERTIES
	hwnd_render_target_properties = {.hwnd = window->hwnd,
	    .pixelSize = {
		.height = size.height,
		.width = size.width,
	    }};

	hr = ID2D1Factory_CreateHwndRenderTarget(
	    window->factory,
	    &render_target_properties,
	    &hwnd_render_target_properties,
	    &window->render_target
	);
	if (!SUCCEEDED(hr))
		return hr;

	D2D1_COLOR_F blue = {
	    .b = 1.0f,
	    .a = 1.0f,
	};

	hr = ID2D1HwndRenderTarget_CreateSolidColorBrush(
	    window->render_target,
	    &blue,
	    NULL,
	    &window->brush
	);

	return hr;
}

void discard_device_resources(Window *window)
{
	if (!window)
		return;

	release((void **)&window->brush);
	release((void **)&window->render_target);
}

void direct2d_deinit(Window *window)
{
	discard_device_resources(window);
	release((void **)&window->factory);
}

static void release(void **obj)
{
	if (obj && *obj) {
		((IUnknown *)*obj)->lpVtbl->Release((IUnknown *)*obj);
		*obj = NULL;
	}
}
