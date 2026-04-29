#include <d2d1.h>
#include <windows.h>
#include "window.h"

HRESULT create_device_independent_resources(ID2D1Factory *factory);
void direct2d_deinit(Window *window);
