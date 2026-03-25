[BITS 64]
    global _start

_start:
    ; 1. SAUVEGARDE TOTALE
    pushfq
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax

    ; 2. AFFICHAGE "....WOODY...."
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel msg]
    mov rdx, 14
    syscall

    ; 3. RESTAURATION TOTALE
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    popfq

    ; 4. CALCUL DU SAUT DE FAÇON ISOLÉE
    ; On push rax et rbx temporairement pour ne pas les salir
    push rax
    push rbx

    lea rax, [rel _start]
    mov rbx, 0x1111222233334444
    sub rax, rbx
    mov rbx, 0x5555666677778888
    add rax, rbx

    ; On déplace l'adresse cible dans r11 (registre sacrifiable)
    mov r11, rax

    ; On restaure rax et rbx dans leur état d'origine
    pop rbx
    pop rax

    ; 5. SAUT FINAL
    jmp r11

msg: db "....WOODY....", 10