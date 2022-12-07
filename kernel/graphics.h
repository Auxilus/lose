#ifndef _GRAPHICS
#define _GRAPHICS

#include "../utils/mem.h"

#define GR_START 0xa0000
#define GR_WIDTH 320
#define GR_HEIGHT 200

#define GR_COLOR_BG 0x12

void gr_init_graphics(void);
void gr_clear_screen(void);
void gr_print_character(int x, int y, int character);
void gr_print_string(int x, int y, char* string, int length);

#endif
