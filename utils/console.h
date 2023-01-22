#ifndef _UTILS_CONSOLE
#define _UTILS_CONSOLE

static volatile int enable_gr_print;

void console_pre_init(void);
void console_pre_print(char *str);

#endif