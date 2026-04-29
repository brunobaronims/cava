#include "windows/window.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <windows.h>

HRESULT create_device_independent_resources(ID2D1Factory *factory)
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(
	    D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    &IID_ID2D1Factory,
	    NULL,
	    (void *)&factory
	);

	return hr;
}

void release(void **obj)
{
	if (obj && *obj) {
		((IUnknown *)*obj)->lpVtbl->Release((IUnknown *)*obj);
		*obj = NULL;
	}
}

void direct2d_deinit(Window *window)
{
	release((void **)&window->brush);
	release((void **)&window->render_target);
	release((void **)&window->factory);
}
