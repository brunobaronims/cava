#include "windows/graphics_d2d.h"
#include <stdio.h>

static void release(void **obj);

HRESULT cava_d2d_init(CavaGraphicsD2D *graphics_d2d)
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(
	    D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    &IID_ID2D1Factory,
	    NULL,
	    (void **)&graphics_d2d->factory
	);

	return hr;
}

HRESULT
cava_d2d_create_device_resources(CavaGraphicsD2D *graphics_d2d, HWND hwnd)
{
	HRESULT hr = S_OK;

	if (graphics_d2d->render_target)
		return hr;

	RECT rc;
	if (!GetClientRect(hwnd, &rc))
		return HRESULT_FROM_WIN32(GetLastError());

	D2D1_SIZE_U size = {
	    .width = rc.right - rc.left,
	    .height = rc.bottom - rc.top,
	};

	D2D1_RENDER_TARGET_PROPERTIES render_target_properties = {0};
	D2D1_HWND_RENDER_TARGET_PROPERTIES
	hwnd_render_target_properties = {.hwnd = hwnd,
	    .pixelSize = {
		.height = size.height,
		.width = size.width,
	    }};

	hr = ID2D1Factory_CreateHwndRenderTarget(
	    graphics_d2d->factory,
	    &render_target_properties,
	    &hwnd_render_target_properties,
	    &graphics_d2d->render_target
	);
	if (!SUCCEEDED(hr))
		return hr;

	D2D1_COLOR_F blue = {
	    .b = 1.0f,
	    .a = 1.0f,
	};

	hr = ID2D1HwndRenderTarget_CreateSolidColorBrush(
	    graphics_d2d->render_target,
	    &blue,
	    NULL,
	    &graphics_d2d->brush
	);

	return hr;
}

void cava_d2d_discard_device_resources(CavaGraphicsD2D *graphics_d2d)
{
	if (!graphics_d2d)
		return;

	release((void **)&graphics_d2d->brush);
	release((void **)&graphics_d2d->render_target);
}

void cava_d2d_deinit(CavaGraphicsD2D *graphics_d2d)
{
	cava_d2d_discard_device_resources(graphics_d2d);
	release((void **)&graphics_d2d->factory);
}

void cava_d2d_on_resize(CavaGraphicsD2D *graphics_d2d, UINT height, UINT width)
{
	if (!graphics_d2d->render_target)
		return;

	D2D1_SIZE_U size = {
	    .height = height,
	    .width = width,
	};
	ID2D1HwndRenderTarget_Resize(graphics_d2d->render_target, &size);
}

HRESULT cava_d2d_on_render(CavaGraphicsD2D *graphics_d2d, HWND hwnd, UINT dpi_scale)
{
	HRESULT hr = S_OK;

	hr = cava_d2d_create_device_resources(graphics_d2d, hwnd);
	if (!SUCCEEDED(hr))
		return hr;

	RECT rc;
	if (!GetClientRect(hwnd, &rc))
		return HRESULT_FROM_WIN32(GetLastError());

	D2D1_SIZE_F size = {
	    .width = (FLOAT)(rc.right - rc.left) / dpi_scale,
	    .height = (FLOAT)(rc.bottom - rc.top) / dpi_scale,
	};

	ID2D1HwndRenderTarget_BeginDraw(graphics_d2d->render_target);

	D2D1_MATRIX_3X2_F transform = {
	    ._11 = 1.0f,
	    ._22 = 1.0f,
	};
	ID2D1HwndRenderTarget_SetTransform(graphics_d2d->render_target, &transform);

	D2D1_COLOR_F white = {
	    .r = 1.0f,
	    .b = 1.0f,
	    .g = 1.0f,
	    .a = 1.0f,
	};
	ID2D1HwndRenderTarget_Clear(graphics_d2d->render_target, &white);

	D2D1_RECT_F rectangle = {
	    .top = size.height / 2 - 50.0f,
	    .bottom = size.height / 2 + 50.0f,
	    .left = size.width / 2 - 50.0f,
	    .right = size.width / 2 + 50.0f,
	};
	printf("left: %f\n", rectangle.left);
	printf("top: %f\n", rectangle.top);

	ID2D1HwndRenderTarget_FillRectangle(
	    graphics_d2d->render_target,
	    &rectangle,
	    (ID2D1Brush *)graphics_d2d->brush
	);

	hr = ID2D1HwndRenderTarget_EndDraw(graphics_d2d->render_target, NULL, NULL);
	if (hr == D2DERR_RECREATE_TARGET) {
		hr = S_OK;
		cava_d2d_discard_device_resources(graphics_d2d);
	}

	return hr;
}

static void release(void **obj)
{
	if (obj && *obj) {
		((IUnknown *)*obj)->lpVtbl->Release((IUnknown *)*obj);
		*obj = NULL;
	}
}
