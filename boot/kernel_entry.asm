	global _start
	[bits 32]
	_start:
		[extern main] ; kernel.c 'main' function
		call main

	; main exited

	jmp $
		
MAINEXIT: db "kernel exited"
