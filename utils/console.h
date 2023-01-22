#ifndef _UTILS_CONSOLE
#define _UTILS_CONSOLE

void console_pre_init(void);
void console_pre_print(char *str);
void console_set_enable_gr_print(int val);
int console_get_enable_gr_print(void);
#endif