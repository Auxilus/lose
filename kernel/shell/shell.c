#include "shell.h"
#include "../graphics.h"
#include "../../utils/mem.h"
#include "../../utils/console.h"
#include "../../cpu/timer.h"
#include "../../drivers/ports.h"
#include "../../drivers/rtc.h"
#include "../../drivers/ata.h"

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
  char *cmd = (char *)malloc(10);
  char *arg = (char *)malloc(20);

  for (int i = 0; i < strlen(shell_buffer); i++)
  {
    if (shell_buffer[i] != ' ')
    {
      cmd[i] = shell_buffer[i];
    }
    else
    {
      if (strlen(shell_buffer) - i + 1 > 0)
      {
        memcpy(&shell_buffer[i + 1], arg, strlen(shell_buffer) - i - 1);
      }
      break;
    }
  }

  if (strcmp(cmd, "clear") == 0)
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

  if (strcmp(cmd, "") == 0)
  {
    return 0;
  }

  if (strcmp(cmd, "info") == 0)
  {
    gr_window_print(SHELL_COMMAND_INFO);
    return 1;
  }

  if (strcmp(cmd, "help") == 0)
  {
    gr_window_print(SHELL_COMMAND_HELP);
    return 1;
  }

  if (strcmp(cmd, "test") == 0)
  {
    console_set_enable_gr_print(1);
    char *buf = (char *)malloc(512);
    int sector = atoi(arg);
    ata_pio_read48(sector, 1, buf);
    console_set_enable_gr_print(0);
    return 1;
  }

  if (strcmp(cmd, "exit") == 0)
  {
    gr_window_print("Shutting down...\n");
    // timer_sleep(5);
    gr_clear_screen();
    port_word_out(0x604, 0x2000);
    return 1;
  }

  if (strcmp(cmd, "time") == 0)
  {
    rtc_time time = read_rtc();
    char timestamp[256];
    sprintf(timestamp, "%02u %02u %02u %02u:%02u:%02u\n",
            time.month, time.day, time.year, time.hour, time.minute, time.second);
    gr_window_print(timestamp);
    return 1;
  }

  // fs commands
  if (strcmp(cmd, "dir") == 0)
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

  if (strcmp(cmd, "pwd") == 0)
  {
    char *pwd = vfs_get_pwd();
    char node_info[30];
    sprintf(node_info, "%s\n", pwd);
    gr_window_print(node_info);
    return 1;
  }

  if (strcmp(cmd, "cd") == 0)
  {
    if (vfs_change_dir(arg) != 1)
    {
      gr_window_print("error: directory not found\n");
    }
    return 1;
  }

  if (strcmp(cmd, "cat") == 0)
  {
    vfs_file *ret = vfs_cat_dir(arg);

    if (ret != NULL)
    {
      gr_window_print("\n");
      gr_window_print(ret->fptr);
      // this is causing the issue in cmd arg parsing. free is ovverriding the cmd buffer
      // free(ret->read_size);
      gr_window_print("\n");
    }
    else
    {
      gr_window_print("error: file read failed\n");
    }

    return 1;
  }

  char unknown[512 + 20];
  sprintf(unknown, "Unknown command %s\n", shell_buffer);
  gr_window_print(unknown);
}