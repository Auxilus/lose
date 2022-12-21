#include "serial.h"

// serial I/O
int serial_init() {
	port_byte_out(SERIAL_COM1 + 1, 0x00);    // Disable interrupts
	port_byte_out(SERIAL_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	port_byte_out(SERIAL_COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	port_byte_out(SERIAL_COM1 + 1, 0x00);    //                  (hi byte)
	port_byte_out(SERIAL_COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
	port_byte_out(SERIAL_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	port_byte_out(SERIAL_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	port_byte_out(SERIAL_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
	port_byte_out(SERIAL_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE)

	// Check if serial is faulty (i.e: not same byte as sent)
	if(port_byte_in(SERIAL_COM1 + 0) != 0xAE) {
		return 1;
	}

	// If serial is not faulty set it in normal operation mode
	// (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
	port_byte_out(SERIAL_COM1 + 4, 0x0F);

	serial_print("SERIAL: serial port enabled [COM1] \n");

	return 0;
}

int serial_received() {
	return port_byte_in(SERIAL_COM1 + 5) & 1;
}

char serial_read() {
	while (serial_received() == 0);

	return port_byte_in(SERIAL_COM1);
}

int serial_is_transmit_empty() {
	return port_byte_in(SERIAL_COM1 + 5) & 0x20;
}

void serial_write(char a) {
	while (serial_is_transmit_empty() == 0);
	port_byte_out(SERIAL_COM1,a);
}

void serial_color_on() {
	char debug[6] = "[0;96m";
	int c = 0;
	serial_write(0x1B);
	while(c < 6) {
		serial_write(debug[c++]);
	}
}

void serial_color_off() {
	char debug[6] = "[0;40m";
	int c = 0;
	serial_write(0x1B);
	while(c < 6) {
		serial_write(debug[c++]);
	}
}

void serial_print(char *message) {
	serial_color_on();

	int c = 0;
	while(message[c] != '\0') {
		serial_write(message[c]);
		c++;
	}

	serial_color_off();
}
