#ifndef _SHELL
#define _SHELL

#include "../../drivers/keyboard.h"

#define SHELL_PROMPT        "]"
#define SHELL_WAIT_CHAR     "_"
#define SHELL_BUFFER_MAX    512
#define SHELL_COMMAND_INFO  "\n---------[LOSE OS, VERSION 1337]---------\n   Tired of government spying on you?\n   try lose!\n"
#define SHELL_COMMAND_HELP  "Suppoted commands: help info dir time clear test\n"

void shell_init(void);
void shell_loop(void);
void shell_keypress(key_event ke);

#endif