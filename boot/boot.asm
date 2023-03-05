; Add BIOS load offset
[org 0x7c00]


KERNEL_OFFSET equ 0x8000 ; This should match the value in Makefile Ttext

    ; store the boot drive additional disk real operations
    ; BIOD will set this in dl
    mov [BOOT_DRIVE], dl

    mov bp, 0x70000
    mov sp, bp

    ; set graphics mode 13h
	;push ax
	;mov ah, 0x00
	;mov al, 0x13
	;int 0x10
	;pop ax

	; set graphics mode 12h (640x480x16)
    ; not sure if ax and bx have to be restored
    ; don't care about the cycles right now, push everything!
	pusha
	mov ax, 0x4f02 ; set VESA-Compliant video modes
	mov bx, 0x12
	int 0x10
	popa

    call load_kernel ; read the kernel from disk
    call detect_pmm
    call switch_to_pm ; disable interrupts, load GDT,  etc. Finally jumps to 'BEGIN_PM'
    jmp $ ; Never executed

%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/switch_pm.asm"
%include "boot/pmm.asm"

[bits 16]
load_kernel:

    ; load kernel at KERNEL_OFFSET
    ; for now, the number of sectors has to be hard-coded, this should be refactored in the future
    mov bx, KERNEL_OFFSET
    mov dh, 62 ; This needs to be changed once in a while
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    ; jump to kernel code
    call KERNEL_OFFSET
    hlt ; don't crash, lol.


BOOT_DRIVE db 0

; boot magic padding
times 510 - ($-$$) db 0
dw 0xaa55
