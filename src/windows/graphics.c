#include "windows/graphics.h"

int cava_graphics_init(CavaGraphics *graphics)
{
	if (!graphics)
		return 1;

	if (!cava_d3d11_init(&graphics->d3d11)) {
		graphics->type = D3D11;
		return 0;
	}

	if (!SUCCEEDED(cava_d2d_init(&graphics->d2d)))
		return 1;

	graphics->type = D2D;
	return 0;
}

void cava_graphics_deinit(CavaGraphics *graphics)
{
	if (!graphics)
		return;

	switch (graphics->type) {
	case D2D: {
		cava_d2d_deinit(&graphics->d2d);
	}
	case D3D11:
		return;
	}
}

void cava_graphics_on_resize(CavaGraphics *graphics, UINT height, UINT width)
{
	if (!graphics)
		return;

	switch (graphics->type) {
	case D2D: {
		cava_d2d_on_resize(&graphics->d2d, height, width);
	}
	case D3D11:
		return;
	}
}

HRESULT
cava_graphics_on_render(CavaGraphics *graphics, HWND hwnd, UINT dpi_scale)
{
	if (!graphics)
		return 0;

	switch (graphics->type) {
	case D2D: {
		return cava_d2d_on_render(&graphics->d2d, hwnd, dpi_scale);
	}
	case D3D11:
		return 0;
	}
	return 0;
}
