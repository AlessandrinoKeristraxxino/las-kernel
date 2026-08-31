; crate/src/drivers/irq_stubs.s

[GLOBAL irq0_stub]
[GLOBAL irq1_stub]

irq0_stub:
    ; salva registri
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call timer_interrupt_handler

    ; reipristina i dati dei registri
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; manda EOI al PIC
    mov al, 0x20
    out 0x20, al

    ; fine interrupt
    iretq


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;ancora da fare il keyboard interrupt;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
irq1_stub:
    ; salva registri
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call keyboard_interrupt_handler

    ; ripristina i dati dei registri
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; manda EOI al PIC
    mov al, 0x20
    out 0x20, al

    ; fine interrupt
    iretq