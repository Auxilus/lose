#include "shell.h"
#include "../../utils/mem.h"
#include "../graphics.h"
#include "../../utils/console.h"
#include "../../cpu/timer.h"
#include "../../drivers/ports.h"

void handle_command(void);

// shell_buffer will have maximum 512 characters.
// line width is 20, so this should be enough
static char *shell_buffer;
static int shell_buffer_loc;
static int isReturn;
void shell_init(void)
{
  isReturn = 0;
  shell_buffer_loc = 0;
  shell_buffer = (char *)malloc(SHELL_BUFFER_MAX);
  console_pre_print("SHELL: init, disabling system log output\n");
  console_set_enable_gr_print(0);
  gr_print('\n');
  gr_window_print(SHELL_PROMPT);
  gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_', 1);
  shell_loop();
}
void shell_loop(void)
{
  while (1)
  {
    if (isReturn)
    {
      handle_command();
      memset(shell_buffer, (u8)'\0', shell_buffer_loc);
      shell_buffer_loc = 0;
      gr_window_print(SHELL_PROMPT);
      gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_', 1);
    }
  }
}
void shell_keypress(key_event ke)
{
  // if (ke.is_shift && ke.is_ctrl && ke.letter == 'Z')
  // {
  // 	// basic shutdown for qemu ctrl+shift+c
  // 	console_pre_print("kernel: kernel shutdown initiated\n");
  // 	gr_clear_screen();
  // 	gr_print_string(10, 10, "shutting down...");
  // 	timer_sleep(1);
  // 	port_word_out(0x604, 0x2000);
  // 	return;
  // }
  // if (ke.is_shift && ke.is_ctrl && ke.letter == 'T')
  // {
  // 	// scroll test
  // 	gr_window_scroll();
  // 	return;
  // }

  // if (ke.is_shift && ke.is_ctrl && ke.letter == 'C')
  // {
  // 	// basic shutdown for qemu ctrl+shift+c
  // 	gr_clear_screen();
  // 	windowctx->cursor_x = 0;
  // 	windowctx->cursor_y = 0;
  // 	return;
  // }

  if (ke.letter == 0x0a)
  {
    isReturn = 1;
    return;
  }
  if (ke.letter == 0x08)
  {
    shell_buffer_loc--;
    if (shell_buffer_loc < 0)
    {
      shell_buffer_loc = 0;
    }
    shell_buffer[shell_buffer_loc] = '\0';
    gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ', 1);
  }
  else
  {
    shell_buffer[shell_buffer_loc++] = ke.letter;
  }
  gr_print(ke.letter);
  gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_', 1);
}

void handle_command()
{
  // clear the wait character
  gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ', 1);
  // print result on new line
  gr_print('\n');
  // reset isReturn
  isReturn = 0;

  if (strcmp(shell_buffer, "info") == 0)
  {
    gr_window_print(SHELL_COMMAND_INFO);
    return;
  }

  if (strcmp(shell_buffer, "exit") == 0)
  {
    gr_window_print("Shutting down...\n");
    timer_sleep(1);
    gr_clear_screen();
    port_word_out(0x604, 0x2000);
    return;
  }

  char unknown[512 + 20];
  sprintf(unknown, "Unknown command %s\n", shell_buffer);
  gr_window_print(unknown);
}