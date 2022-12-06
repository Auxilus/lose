;; use VGA memory to directly write the characters to the screen
;; VGA memory starts at 0xb8000
;; rach character uses 2 bytes - ASCII and color
		[bits 32]

		VIDEO_MEMORY equ 0xb8000
		WHITE_ON_BLACK equ 0x0f		; default color byte

print_string_pm:
		pusha
		; init pointer location
		mov edx, VIDEO_MEMORY

print_string_pm_loop:
		mov al, [ebx]			; use ebx to point to the string location in memory
		mov ah, WHITE_ON_BLACK

		cmp al, 0
		je print_string_pm_end

		mov [edx], ax			; move the charater+color to video memory
		inc ebx
		inc edx
		
		jmp print_string_pm_loop

print_string_pm_end:
		popa
		ret
