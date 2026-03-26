[BITS 64]
    global _start

_start:
    ; 1. SAUVEGARDE MINIMALE
    ; On ne sauve que ce que 'write' et 'syscall' vont modifier
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r11

    ; 2. AFFICHAGE "....WOODY...."
    mov rax, 1          ; syscall: write
    mov rdi, 1          ; fd: stdout
    lea rsi, [rel msg]  ; buf: message
    mov rdx, 14         ; count: 14
    syscall

    ; 3. RESTAURATION
    pop r11
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax

    ; 4. CALCUL DU SAUT (Utilise r11 et rcx qui sont volatils)
    lea r11, [rel _start]
    mov rcx, 0x1111222233334444 ; Signature vaddr (Patchée en C)
    sub r11, rcx
    mov rcx, 0x5555666677778888 ; Signature OEP (Patchée en C)
    add r11, rcx

    ; 5. JUMP
    jmp r11

msg: db "....WOODY....", 10