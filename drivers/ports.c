#include "ports.h"

u8 port_byte_in (u16 port) {
	u8 result;
	/* Inline assembler syntax
	 * !! Notice how the source and destination registers are switched from NASM !!
	 *
	 * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
	 * '"d" (port)': map the C variable '(port)' into e'd'x register
	 *
	 * Inputs and outputs are separated by colons
	 */
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void port_byte_out (u16 port, u8 data) {
	/* Notice how here both registers are mapped to C variables and
	 * nothing is returned, thus, no equals '=' in the asm syntax 
	 * However we see a comma since there are two variables in the input area
	 * and none in the 'return' area
	 */
	__asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

u16 port_word_in (u16 port) {
	u16 result;
	__asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void port_word_out (u16 port, u16 data) {
	__asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

// serial I/O
int init_serial() {
	port_byte_out(SERIAL_COM1 + 1, 0x00);    // Disable all interrupts
	port_byte_out(SERIAL_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	port_byte_out(SERIAL_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	port_byte_out(SERIAL_COM1 + 1, 0x00);    //                  (hi byte)
	port_byte_out(SERIAL_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
	port_byte_out(SERIAL_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	port_byte_out(SERIAL_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	port_byte_out(SERIAL_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
	port_byte_out(SERIAL_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

	// Check if serial is faulty (i.e: not same byte as sent)
	if(port_byte_in(SERIAL_COM1 + 0) != 0xAE) {
		return 1;
	}

	// If serial is not faulty set it in normal operation mode
	// (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
	port_byte_out(SERIAL_COM1 + 4, 0x0F);
	return 0;
}

int serial_received() {
	return port_byte_in(SERIAL_COM1 + 5) & 1;
}

char read_serial() {
	while (serial_received() == 0);

	return port_byte_in(SERIAL_COM1);
}

int is_transmit_empty() {
	return port_byte_in(SERIAL_COM1 + 5) & 0x20;
}

void write_serial(char a) {
	while (is_transmit_empty() == 0);
	port_byte_out(SERIAL_COM1,a);
}

void print_serial(char *message) {
	int c = 0;
	while(message[c++] != '\0') {
		write_serial(message[c]);
	}
}
