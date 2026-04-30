#include "cava.h"

int main(void)
{
	Cava cav = {0};
	int result = cava_init(&cav);
	if (result)
		return result;

	cava_run();

	cava_deinit(&cav);

	return 0;
}
