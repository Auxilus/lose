		global _start
		[bits 32]
		_start:
			[extern main] ; Define calling point. Must have same name as kernel.c 'main' function
			call main ; Calls the C function. The linker will know where it is placed in memory

		; main exited

		;mov ebx, MAINEXIT
		;call print_string_pm

		;jmp $

		%include "boot/32bit_print.asm"
		
MAINEXIT: db "kernel exited"
