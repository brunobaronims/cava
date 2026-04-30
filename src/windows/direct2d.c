#include "windows/direct2d.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <windows.h>
#include <winerror.h>

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

	if (!window->render_target) {
		RECT rc;
		GetClientRect(window->hwnd, &rc);

		D2D1_SIZE_U size = {
		    .width = rc.right - rc.left,
		    .height = rc.top - rc.bottom,
		};

		const D2D1_RENDER_TARGET_PROPERTIES properties = {0};
		const D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_properties = {
		    .hwnd = window->hwnd,
		    .pixelSize = {
			.height = size.height,
			.width = size.width,
		    }};

		if (!SUCCEEDED(window->factory->lpVtbl->CreateHwndRenderTarget(
			window->factory,
			&properties,
			&hwnd_properties,
			&window->render_target
		    ))) {
			return 0;
		}
	}

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
