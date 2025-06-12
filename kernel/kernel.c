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
#include "process.h"
#include "syscall.h"

void dummy()
{
}

void kernel_handle_key(key_event ke)
{
        shell_keypress(ke);
}

static void init_process(void)
{
        vga_write_registers();
        gr_init_graphics();
        console_set_enable_gr_print(1);

        rtc_time time = read_rtc();
        char timestamp[80];
        sprintf(timestamp, "KERNEL: time %02u/%02u/%02u %02u:%02u:%02u\n",
                        time.month, time.day, time.year,
                        time.hour, time.minute, time.second);
        console_pre_print(timestamp);

        console_pre_print("KERNEL: initializing vfs\n");
        fs_node *root_node = vfs_init();
        char vfs_msg[20];
        sprintf(vfs_msg, "KERNEL: %s\n", ((fs_node *)root_node->volume)->name);
        console_pre_print(vfs_msg);

        rtl8139_init();

        shell_init();

        while (1)
                asm volatile("hlt");
}

void main()
{
	console_set_enable_gr_print(0);
	serial_init();
        process_init();
        syscall_init();
        register_syscall(0, syscall_print);

        process_create(init_process);

        isr_install();
        irq_install();
        pmm_dump();
        acpi_init();
        pci_init();

        while (1)
                asm volatile("hlt");
}