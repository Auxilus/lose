/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */

#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../drivers/acpi.h"
#include "../drivers/ports.h"
#include "../drivers/serial.h"
#include "../drivers/memory.h"
#include "../drivers/rtc.h"
#include "../utils/mem.h"
#include "../utils/console.h"
#include "../utils/string.h"
#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "./graphics.h"
#include "kernel.h"
#include "shell/shell.h"

void dummy_test_entrypoint()
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

	rtc_time time = read_rtc();

	char timestamp[256];
	sprintf(timestamp, "KERNEL: %02u/%02u/%02u %02u:%02u:%02u\n",
					time.month, time.day, time.year, time.hour, time.minute, time.second);
	console_pre_print(timestamp);
	
	shell_init();

	// gr_print_string(520, 430, "VGA mode 0x12\nfont test\n640x480x16");
	while (1)
		;
}
