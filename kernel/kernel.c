/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../drivers/acpi.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../drivers/memory.h"
#include "../utils/mem.h"
#include "../utils/console.h"
#include "../utils/string.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "./graphics.h"
#include "kernel.h"

void dummy_test_entrypoint()
{
}

void kernel_handle_key(key_event ke)
{
	if (ke.is_shift && ke.is_ctrl && ke.letter == 'Z')
	{
		// basic shutdown for qemu ctrl+shift+c
		console_pre_print("kernel: kernel shutdown initiated\n");
		gr_clear_screen();
		gr_print_string(10, 10, "shutting down...");
		timer_sleep(1);
		port_word_out(0x604, 0x2000);
		return;
	}
	if (ke.is_shift && ke.is_ctrl && ke.letter == 'T')
	{
		// scroll test
		gr_window_scroll();
		return;
	}

	if (ke.is_shift && ke.is_ctrl && ke.letter == 'C')
	{
		// basic shutdown for qemu ctrl+shift+c
		gr_clear_screen();
		windowctx->cursor_x = 0;
		windowctx->cursor_y = 0;
		return;
	}

	gr_print(ke.letter);
}

void main()
{
	console_set_enable_gr_print(0);
	serial_init();
	vga_write_registers();
	gr_init_graphics();
	console_set_enable_gr_print(1);
	isr_install();
	irq_install();

	console_pre_print("KERNEL: testing interrupts\n");
	asm("int $2");
	asm("int $3");
	console_pre_print("KERNEL: interrupt test complete\n");

	console_pre_print("KERNEL: testing malloc\n");
	int *a = (int *)malloc(sizeof(int) * 5);
	char *s = (char *)malloc(64);

	char message1[128];
	sprintf(message1, "KERNEL: int[5]   0x%p\n", a);
	char message2[128];
	sprintf(message2, "KERNEL: char[64] 0x%p\n", s);

	console_pre_print(message1);
	console_pre_print(message2);
	pmm_dump();
	acpi_init();
	pci_init();


	// gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");
	while (1)
		;
}
