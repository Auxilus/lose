#ifndef _GRAPHICS
#define _GRAPHICS

#include "../utils/mem.h"
#include "../utils/string.h"

#define GR_START 0xa0000
#define GR_WIDTH 640
#define GR_HEIGHT 480

typedef struct {
	int cursor_x;
	int cursor_y;
	char *charbuf;
} winctx;

static winctx *windowctx;

void gr_init_graphics(void);
void gr_clear_screen(void);
void gr_print_character(int x, int y, int character, int skipAdvance);
void gr_print_string(int x, int y, char* string);
void gr_print(char character);
void gr_window_print(const char *string);
void gr_window_scroll(void);
int gr_window_get_buffer_idx(int x, int y);
int gr_window_get_x(int idx);
int gr_window_get_y(int idx);

void gr_draw_line(int x0, int y0, int x1, int y1, char color);
void gr_draw_rect(int x0, int y0, int w, int l, char color);

#endif
