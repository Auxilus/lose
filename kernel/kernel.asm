    ;; Set video mode
    mov ah, 0x00                ; int 0x10/ ah 0x00 = set video mode
    mov al, 0x12                ; 640x480 256 color
    int 0x10

    ;; Change color/Palette
    ;; mov ah, 0x0B
    ;; mov bh, 0x00
    ;; mov bl, 0x01
    ;; int 0x10

    mov si, testString
    call print_string
		cli

    hlt                         ; halt the cpu

print_string:
    mov ah, 0x0e
    mov bh, 0x0
    mov bl, 0x07 

print_char:
    mov al, [si]
    cmp al, 0
    je end_print
    int 0x10
    inc si
    jmp print_char

end_print:
    ret

testString:     db 'Kernel booted', 0xA, 0xD, 0 

    ;; Sector Padding magic
    times 512-($-$$) db 0       ; pads out 0s until we reach 512th byte
