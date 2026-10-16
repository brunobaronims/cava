#include "cava.h"

int cava_init(Cava *cava)
{
	if (!cava)
		return 1;

	if (cava_window_init(&cava->window))
		return 1;

	return 0;
}

void cava_deinit(Cava *cava)
{
	if (!cava)
		return;

	cava_window_deinit(&cava->window);
}
