#include "cava.h"

int cava_init(Cava *cava)
{
	if (!cava)
		return 1;

	if (window_init(&cava->window))
		return 1;

	return 0;
}

void cava_deinit(Cava *cava)
{
	if (!cava)
		return;

	window_deinit(&cava->window);
}

void cava_run(void) {
	window_run();
}
