#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../drivers/acpi.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../drivers/memory.h"
#include "../drivers/rtl8139.h"
#include "../drivers/rtc.h"
#include "../utils/mem.h"
#include "../utils/console.h"
#include "../utils/string.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "./graphics.h"
#include "kernel.h"
#include "fs/vfs.h"
#include "shell/shell.h"

void dummy()
{
}

void kernel_handle_key(key_event ke)
{
	shell_keypress(ke);
}

void main()
{
	console_set_enable_gr_print(0);
	serial_init();
	vga_write_registers();
	gr_init_graphics();
	console_set_enable_gr_print(1);
	rtc_time time = read_rtc();
	char timestamp[80];
	sprintf(timestamp, "KERNEL: time %02u/%02u/%02u %02u:%02u:%02u\n",
					time.month, time.day, time.year, time.hour, time.minute, time.second);

	isr_install();
	irq_install();
	pmm_dump();
	acpi_init();
	pci_init();

	console_pre_print("KERNEL: testing interrupts\n");
	asm("int $2");
	asm("int $3");
	console_pre_print("KERNEL: interrupt test complete\n");

	console_pre_print("KERNEL: testing malloc\n");
	char *s = (char *)malloc(32);
	char malloc_test[128];
	sprintf(malloc_test, "KERNEL: char[32] 0x%p\n", s);
	console_pre_print(malloc_test);
	console_pre_print("KERNEL: malloc triggered\n");

	console_pre_print(timestamp);

	console_pre_print("KERNEL: initializing vfs\n");
	fs_node *root_node = vfs_init();
	char vfs_msg[20];
	sprintf(vfs_msg, "KERNEL: %s\n", ((fs_node *)root_node->volume)->name);
	console_pre_print(vfs_msg);

	// init rtl8139 network card
	rtl8139_init();

	// drop to shell
	shell_init();

	// gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");
	// while (1)
	// 	;
	asm volatile("hlt");
}