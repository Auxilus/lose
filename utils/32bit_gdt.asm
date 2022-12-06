;; WHAT THE HELL is GDT?!

gdt_start:
		; GDT starts with a null 8 byte
		dd 0x0
		dd 0x0

; code segment
; base = 0x00000000 length = 0xfffff
gdt_code:
		dw 0xffff			; segment length bits 0-15
		dw 0x0				; segment base   bits 0-15
		db 0x0				; segment base   bits 16-23
		db 10011010b	; flags 8 bits
		db 11001111b	; flags 4 bits + segment length bits 16-19
		db 0x0				; segment base	 bits 24-31

; data segment
; base = 0x00000000 length = 0xfffff
gdt_data:
		dw 0xffff			; segment length bits 0-15
		dw 0x0				; segment base   bits 0-15
		db 0x0				; segment base   bits 16-23
		db 10010010b	; flags 8 bits
		db 11001111b	; flags 4 bits + segment length bits 16-19
		db 0x0				; segment base	 bits 24-31

gdt_end:

;; GDT descriptor
gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
