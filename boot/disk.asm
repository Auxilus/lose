; ref.: https://github.com/cfenollosa/os-tutorial/tree/master/07-bootsector-disk
; load 'dh' sectors from drive 'dl' into ES:BX
disk_load:
    mov [DRIVE], dl ; save drive number
    mov [TOTALREAD], dh
    call get_disk_geometry ; get disk geometry
    mov byte [CURSECTOR], 0x01
read_loop:
    call LBAtoCHS
    call disk_load2
    pusha
    xor eax, eax
    mov al, [CURSECTOR]
    inc al
    mov [CURSECTOR], al ; increment current sector
    xor eax, eax
    mov al, [NREAD]
    inc al
    mov [NREAD], al ; increment number of sectors read
    popa
    add bx, 0x200 ; increment buffer pointer by 512 bytes (1 sector)
    cmp byte [NREAD], 61 ; compare with total number of sectors to read
    jl read_loop ; if not done, continue reading
read_done:
    ret

disk_load2:
    pusha
    ; reading from disk requires setting specific values in all registers
    ; so we will overwrite our input parameters from 'dx'. Let's save it
    ; to the stack for later use.
    push dx

    mov ah, 0x02 ; ah <- int 0x13 function. 0x02 = 'read'
    mov al, 0x01   ; al <- number of sectors to read (0x01 .. 0x80)
    mov cl, [PHYSECTOR] ; cl <- sector (0x01 .. 0x11)
                 ; 0x01 is our boot sector, 0x02 is the first 'available' sector
    mov ch, [CYLINDER] ; ch <- cylinder (0x0 .. 0x3FF, upper 2 bits in 'cl')
    ; dl <- drive number. Our caller sets it as a parameter and gets it from BIOS
    ; (0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2)
    mov dh, [HEAD] ; dh <- head number (0x0 .. 0xF)

    ; [es:bx] <- pointer to buffer where the data will be stored
    ; caller sets it up for us, and it is actually the standard location for int 13h
    int 0x13      ; BIOS interrupt
    jc disk_error ; if error (stored in the carry bit)

    pop dx
    cmp al, 0x01    ; BIOS also sets 'al' to the # of sectors read. Compare it.
    jne sectors_error
    popa
    ret

disk_error:
    ;mov dh, ah ; ah = error code, dl = disk drive that dropped the error
    ;call print_hex ; check out the code at http://stanislavs.org/helppc/int_13-1.html
    jmp disk_loop

sectors_error:
    ;mov bx, SECTORS_ERROR
    ;call print

disk_loop:
    jmp $

get_disk_geometry:
    pusha
    push es
    mov ah, 0x08
    int 0x13
    and cx, 0x3f ; save max sectors
    mov [SECTORSPERCYLINDER], cl
    add dh, 1 ; save max heads
    mov [MAXHEAD], dh
    pop es
    popa
    ret

LBAtoCHS:
    pusha
    push es
    xor ax, ax
    mov al, [CURSECTOR]
    xor dx, dx
    xor bx, bx
    mov bx, [SECTORSPERCYLINDER]
    div bx
    inc dl
    mov [PHYSECTOR], dl
    xor dx, dx
    xor bx, bx
    mov bx, [MAXHEAD]
    div bx
    mov byte [HEAD], dl
    mov byte [CYLINDER], al
    pop es
    popa
    ret

SECTORSPERCYLINDER: dw 0x00
MAXHEAD: db 0
CURSECTOR: db 0x00 ; current sector, used by disk_load2
PHYSECTOR: db 0x00
CYLINDER: db 0x00
HEAD: db 0x00
NREAD: db 0x00
DRIVE: db 0x00
TOTALREAD: db 0x00 ; total number of sectors read, used by disk_load