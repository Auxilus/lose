#ifndef _SHELL
#define _SHELL

#include "../../drivers/keyboard.h"

#define SHELL_PROMPT "]"
#define SHELL_WAIT_CHAR "_"
#define SHELL_BUFFER_MAX 512

void shell_init(void);
void shell_loop(void);
void shell_keypress(key_event ke);

#endif