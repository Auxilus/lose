#include "graphics.h"

void gr_init_graphics(void)
{
	gr_clear_screen();
}

void gr_clear_screen(void)
{
	memset(GR_START, GR_COLOR_BG, GR_WIDTH*GR_HEIGHT);
}
