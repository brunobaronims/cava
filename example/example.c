#include "cava.h"
#include <stdio.h>

int main(void)
{
	Cava cav;
	int result = cava_init(&cav);

	printf("result: %d\n", result);

	cava_run();

	cava_deinit(&cav);

	return 0;
}
