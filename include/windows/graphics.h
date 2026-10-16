#pragma once
#include "graphics_d3d11.h"
#include "graphics_d2d.h"

typedef enum { D3D11, D2D } CavaGraphicsType;

typedef struct {
	CavaGraphicsType type;
	union {
		CavaGraphicsD3D11 d3d11;
		CavaGraphicsD2D d2d;
	};
} CavaGraphics;


int cava_graphics_init(CavaGraphics *graphics);
void cava_graphics_deinit(CavaGraphics *graphics);
void cava_graphics_on_resize(CavaGraphics *graphics, UINT height, UINT width);
HRESULT cava_graphics_on_render(CavaGraphics *graphics, HWND hwnd, UINT dpi_scale);
