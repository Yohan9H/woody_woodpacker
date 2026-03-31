[BITS 64]
    global _start

_start:
    ; 1. SAUVEGARDE TOTALE
    pushfq
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; 2. AFFICHAGE "....WOODY...."
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel msg]
    mov rdx, 14
    syscall

    ; 3. CALCUL DE LA BASE ADDRESS (ASLR / PIE)
    lea r12, [rel _start]
    mov r13, 0x1111222233334444 ; <--- Signature Vaddr Stub
    sub r12, r13                ; R12 = Base Address réelle

    ; 4. MPROTECT (Rendre la zone écrivable)
    mov rdi, 0x1122334455667788 ; <--- Signature Vaddr Segment
    add rdi, r12                ; Adresse réelle = vaddr + base
    
    mov r14, rdi                ; On garde l'adresse exacte pour le XOR
    and rdi, ~0xFFF             ; Alignement sur page (mprotect le réclame)

    mov rsi, 0xDEADBEEFCAFEBABE ; <--- Signature Memsz
    add rsi, 4096               ; Sécurité d'alignement
    mov rdx, 7                  ; PROT_READ | PROT_WRITE | PROT_EXEC
    mov rax, 10                 ; syscall: mprotect
    syscall

    ; 5. BOUCLE DE DÉCHIFFREMENT XOR
    mov rdi, r14                ; Point de départ (adresse déchiffrée)
    mov rcx, 0xDEADBEEFCAFEBABE ; <--- Signature Memsz
    mov r8,  0x0102030405060708 ; <--- Signature Clé XOR
    xor r9, r9                  ; Index i = 0

decrypt_loop:
    cmp r9, rcx
    jge end_decrypt
    
    mov r10, r9
    and r10, 7                  ; r10 = index % 8

    mov rbx, r8                 ; On prend la clé
    push rcx                    
    mov rcx, r10
    shl rcx, 3                  ; On décale pour isoler l'octet
    shr rbx, cl                 
    pop rcx
    
    xor byte [rdi + r9], bl     ; Déchiffrement
    
    inc r9
    jmp decrypt_loop

end_decrypt:
    ; 6. RESTAURATION
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    popfq

    ; 7. CALCUL DU SAUT OEP
    lea r11, [rel _start]
    mov rcx, 0x1111222233334444 ; <--- Signature Vaddr Stub
    sub r11, rcx
    mov rcx, 0x5555666677778888 ; <--- Signature OEP
    add r11, rcx
    jmp r11                     ; Saut vers le vrai Entry Point

msg: db "....WOODY....", 10