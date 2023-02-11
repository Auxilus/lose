#include "shell.h"
#include "../graphics.h"
#include "../../utils/mem.h"
#include "../../utils/console.h"
#include "../../cpu/timer.h"
#include "../../drivers/ports.h"
#include "../../drivers/rtc.h"

// TEST COMMAND INCLUDES
#include "../fs/fat12.h"
#include "../fs/vfs.h"
//

int handle_command(void);

// shell_buffer will have maximum 512 characters.
// line width is 20, so this should be enough
static char *shell_buffer;
static char *last_command;
static int shell_buffer_loc;
static int isReturn;

void shell_init(void)
{
  isReturn = 0;
  shell_buffer_loc = 0;
  shell_buffer = (char *)malloc(SHELL_BUFFER_MAX);
  last_command = (char *)malloc(SHELL_BUFFER_MAX);
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
      int ret = handle_command();
      if (ret)
      {
        memcpy(shell_buffer, last_command, SHELL_BUFFER_MAX);
      }
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
  // UP: buffer in last command
  if (ke.scancode == 72)
  {
    if (strlen(last_command) > 0)
    {
      memcpy(last_command, shell_buffer, SHELL_BUFFER_MAX);
      windowctx->cursor_x = 0;
      shell_buffer_loc = strlen(shell_buffer);
      gr_window_print(SHELL_PROMPT);
      gr_window_print(shell_buffer);
      gr_print_character(windowctx->cursor_x, windowctx->cursor_y, '_', 1);
    }
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

int handle_command()
{
  if (strcmp(shell_buffer, "clear") == 0)
  {
    gr_clear_screen();
    windowctx->cursor_x = 0;
    windowctx->cursor_y = 0;
    // clear the wait character
    gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ', 1);
    // print result on new line
    gr_print('\n');
    // reset isReturn
    isReturn = 0;
    return 0;
  }
  // clear the wait character
  gr_print_character(windowctx->cursor_x, windowctx->cursor_y, ' ', 1);
  // print result on new line
  gr_print('\n');
  // reset isReturn
  isReturn = 0;

  if (strcmp(shell_buffer, "") == 0)
  {
    return 0;
  }

  if (strcmp(shell_buffer, "info") == 0)
  {
    gr_window_print(SHELL_COMMAND_INFO);
    return 1;
  }

  if (strcmp(shell_buffer, "help") == 0)
  {
    gr_window_print(SHELL_COMMAND_HELP);
    return 1;
  }

  if (strcmp(shell_buffer, "test") == 0)
  {
    console_set_enable_gr_print(1);
    fs_node *top_node = vfs_get_top_node();
    for (int i = 0; i < top_node->node_count; i++)
    {
      console_pre_print(((fs_node *)top_node->child[i])->name);
      console_pre_print("\n");
    }
    console_set_enable_gr_print(0);
    return 1;
  }

  if (strcmp(shell_buffer, "exit") == 0)
  {
    gr_window_print("Shutting down...\n");
    // timer_sleep(1);
    gr_clear_screen();
    port_word_out(0x604, 0x2000);
    return 1;
  }

  if (strcmp(shell_buffer, "time") == 0)
  {
    rtc_time time = read_rtc();
    char timestamp[256];
    sprintf(timestamp, "%02u %02u %02u %02u:%02u:%02u\n",
            time.month, time.day, time.year, time.hour, time.minute, time.second);
    gr_window_print(timestamp);
    return 1;
  }

  // fs commands
  if (strcmp(shell_buffer, "dir") == 0)
  {
    fs_node *top_node = vfs_get_top_node();
    gr_window_print("\nTYPE       SIZE CLUSTER    NAME    EXT\n");
    gr_window_print("--------------------------------------\n");
    for (int i = 0; i < top_node->node_count; i++)
    {
      fs_node *node = (fs_node *)top_node->child[i];
      char node_info[30];
      // uint8_t type;
      // uint8_t name[12];
      // int node_count;
      // uint32_t size;
      // struct fs_node **child;
      // struct fs_node *parent;
      // struct fs_node *volume;
      sprintf(node_info, "%c    %10u    %4u    %s\n", vfs_get_node_type(node),
              node->size,
              node->first_cluster,
              node->name);
      gr_window_print(node_info);
    }
    return 1;
  }

  if (strcmp(shell_buffer, "pwd") == 0)
  {
    char *pwd = vfs_get_pwd();
    char node_info[30];
    sprintf(node_info, "%s\n", pwd);
    gr_window_print(node_info);
    return 1;
  }

  char unknown[512 + 20];
  sprintf(unknown, "Unknown command %s\n", shell_buffer);
  gr_window_print(unknown);
}