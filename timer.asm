    bits 16
    cpu 8086

segment _TEXT class=CODE

    global timer_init_
timer_init_:
    push ax
    push es
    push ds

    mov ax, 3508h
    int 21h
    mov [prev_isr], bx
    mov [prev_isr + 2], es

    cli

    mov ax, 2508h
    mov dx, timer_isr ; ds is already equal to cs
    int 21h

    ; Set to 291.304 Hz
    mov dx, 0x43
    mov al, 0x36
    out dx, al
    mov dx, 0x40
    mov al, 0x00 ; Divisor LSB
    out dx, al
    mov al, 0x10 ; MSB
    out dx, al

    sti

    pop ds
    pop es
    pop ax
    ret

    global timer_cleanup_
timer_cleanup_:
    push ax
    push dx
    push ds

    cli

    mov dx, [prev_isr]
    mov ds, [prev_isr + 2]
    mov ax, 2508h
    int 21h

    mov dx, 0x43
    mov al, 0x36
    out dx, al
    mov dx, 0x40
    mov al, 0
    out dx, al
    out dx, al

    sti

    pop ds
    pop dx
    pop ax
    ret

    extern timer_tick_

timer_isr:
    push ax
    push bx
    push cx
    push dx
    push sp
    push bp
    push si
    push di
    push ds
    push es

    mov ax, cs
    mov ds, ax
    mov es, ax

    mov ax, word [cs:clock_counter]
    dec ax
    call timer_tick_

    pop es
    pop ds
    pop di
    pop si
    pop bp
    pop sp
    pop dx
    pop cx
    pop bx
    pop ax

    dec word [cs:clock_counter]
    jz clock_update

    push ax
    mov al, 0x20
    out 0x20, al
    pop ax

    iret

clock_update:
    mov word [cs:clock_counter], 0x10
    jmp far [cs:prev_isr]

    align 4
prev_isr dd 0
clock_counter dw 0x10
