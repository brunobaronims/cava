#pragma once
#include <d3d11.h>

typedef struct {
	int etc;
} CavaGraphicsD3D11;

int cava_d3d11_init(CavaGraphicsD3D11 *graphics_d3d11);
