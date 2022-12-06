    [org 0x7c00]

		mov [DSKNUM], dl

    ;; set up ES:BX memory address/segment:offset to load sector(s) into
    mov bx, 0x1000
    mov es, bx                  
    mov bx, 0x0

    ;; Set up disk read
    mov dh, 0x0                 ; head 0
    mov dl, [DSKNUM]            ; drive 0
    mov ch, 0x0                 ; cylinder 0
    mov cl, 0x02                ; starting sector to read from disk

read_disk:
    mov ah, 0x02                ; BIOS int 13h/ah=2 read disk sectors
    mov al, 0x01                ; # of sectors to read
    int 0x13                    ; BIOS interrupts for disk functions

    jc read_disk

    ;; reset segment registers for RAM
    mov ax, 0x1000
    mov ds, ax                  ; data segment
    mov es, ax                  ; extra segment
    mov fs, ax                  ; ""
    mov gs, ax                  ; ""
    mov ss, ax                  ; stack segment

		;; set video mode beforehand
		;; (because I couldn't get it to set from kernel.c)
		;; me noob. haha
		push ax
		mov ah, 0x00
		mov al, 0x13
		int 0x10
		pop ax
    jmp 0x1000:0x0              ; never return from this!

    ;; Included files
    %include 'utils/print_string.asm'
    %include 'utils/disk_load.asm'

DSKNUM: db 0

    times 510-($-$$) db 0
    dw 0xaa55
