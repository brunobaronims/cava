#pragma once
#include <d2d1.h>

typedef struct {
	ID2D1Factory *factory;
	ID2D1HwndRenderTarget *render_target;
	ID2D1SolidColorBrush *brush;
} CavaGraphicsD2D;

HRESULT cava_d2d_init(CavaGraphicsD2D *graphics_d2d);
HRESULT cava_d2d_create_device_resources(CavaGraphicsD2D *graphics_d2d, HWND hwnd);
void cava_d2d_discard_device_resources(CavaGraphicsD2D *graphics_d2d);
void cava_d2d_on_resize(CavaGraphicsD2D *graphics_d2d, UINT height, UINT width);
HRESULT cava_d2d_on_render(CavaGraphicsD2D *graphics_d2d, HWND hwnd, UINT dpi_scale);
void cava_d2d_deinit(CavaGraphicsD2D *graphics_d2d);
