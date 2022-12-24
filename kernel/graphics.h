#ifndef _GRAPHICS
#define _GRAPHICS

#include "../utils/mem.h"
#include "../utils/string.h"

#define GR_START 0xa0000
#define GR_WIDTH 640
#define GR_HEIGHT 480

#define TRANSPARENT		0xFF
#define BLACK					0
#define BLUE					1
#define GREEN					2
#define CYAN					3
#define RED						4
#define PURPLE				5
#define BROWN					6
#define LTGRAY				7
#define DKGRAY				8
#define LTBLUE				9
#define LTGREEN				10
#define LTCYAN				11
#define LTRED					12
#define LTPURPLE			13
#define YELLOW				14
#define WHITE					15

typedef struct {
	int cursor_x;
	int cursor_y;
} winctx;

static winctx *windowctx;

void gr_init_graphics(void);
void gr_clear_screen(void);
void gr_print_character(int x, int y, int character);
void gr_print_string(int x, int y, char* string);
void gr_print(char character);

void gr_draw_line(int x0, int y0, int x1, int y1, char color);
void gr_draw_rect(int x0, int y0, int w, int l, char color);

#endif
