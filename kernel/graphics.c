#include "graphics.h"

void init_graphics(void)
{
	memset(GR_START, 0x12, GR_WIDTH*GR_HEIGHT);
}
