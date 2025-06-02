; ref.: https://github.com/cfenollosa/os-tutorial/tree/master/07-bootsector-disk
; load 'dh' sectors from drive 'dl' into ES:BX
disk_load:
    mov [DRIVE], dl ; save drive number
    mov [TOTALREAD], dh ; save total number of sectors to read
    call get_disk_geometry ; get disk geometry
    mov byte [CURSECTOR], 0x01 ; start reading from sector 1 (boot sector is 0)

read_loop:
    call LBAtoCHS
    call read_sector
    inc byte [CURSECTOR] ; increment current logical sector
    inc byte [NREAD] ; increment number of sectors read
    add bx, 0x200 ; increment buffer address by 512 bytes (1 sector)
    mov al, [TOTALREAD]
    cmp byte [NREAD], al ; compare with total number of sectors to read
    jl read_loop ; if not done, continue reading

read_done:
    ret

read_sector:
    pusha
    push dx

    mov ah, 0x02 ; 0x02 = 'read'
    mov al, 0x01 ; number of sectors to read (0x01)
    mov cl, [PHYSECTOR] ; sector
    mov ch, [CYLINDER] ; cylinder
    ; mov dl, [DRIVE] ; drive number
    mov dh, [HEAD] ; head

    int 0x13
    jc disk_error

    pop dx
    cmp al, 0x01 ; make sure that we actually read one sector
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
    push es ; int 0x13 seems to modify this
    mov ah, 0x08 ; 0x08 = get drive parameters
    int 0x13
    and cx, 0x3f ; max sectors
    mov [SECTORSPERCYLINDER], cl
    add dh, 1 ; max heads
    mov [MAXHEAD], dh
    pop es
    popa
    ret

; converts logical block address (LBA) to cylinder/head/sector (CHS)
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
    mov [HEAD], dl
    mov [CYLINDER], al
    pop es
    popa
    ret

SECTORSPERCYLINDER: dw 0x00
MAXHEAD: db 0
CURSECTOR: db 0x00 ; current sector, used by read_sector
PHYSECTOR: db 0x00
CYLINDER: db 0x00
HEAD: db 0x00
NREAD: db 0x00
DRIVE: db 0x00
TOTALREAD: db 0x00 ; total number of sectors read, used by disk_load