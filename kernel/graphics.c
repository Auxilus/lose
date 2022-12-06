#include "graphics.h"

void init_graphics(void)
{
	memset(0xa0000, 0x12, 320*200);
}
