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
    
    mov r14, rdi                ; r14 garde l'adresse exacte pour le déchiffrement
    and rdi, ~0xFFF             ; Alignement sur page pour mprotect

    mov rsi, 0xDEADBEEFCAFEBABE ; <--- Signature Memsz
    add rsi, 4096               ; Sécurité d'alignement
    mov rdx, 7                  ; PROT_READ | PROT_WRITE | PROT_EXEC
    mov rax, 10                 ; syscall: mprotect
    syscall

    ; ==========================================================
    ; 5. DECHIFFREMENT RC4
    ; ==========================================================
    
    mov r15, 0xDEADBEEFCAFEBABE ; r15 = Taille du segment
    mov r8,  0x0102030405060708 ; r8  = Clé RC4 (8 octets)

    ; a) Allocation sur la pile
    push r8                     ; On stocke la clé sur la pile (8 octets)
    sub rsp, 256                ; On alloue 256 octets pour la S-Box

    ; b) KSA Étape 1 : Initialisation de la S-Box (S[i] = i)
    xor rcx, rcx                ; rcx (i) = 0
init_sbox:
    cmp rcx, 256
    jge ksa_mix
    mov byte [rsp + rcx], cl    ; S[i] = i
    inc rcx
    jmp init_sbox

    ; c) KSA Étape 2 : Mélange de la S-Box avec la clé
ksa_mix:
    xor rcx, rcx                ; i = 0
    xor r10, r10                ; j = 0
ksa_loop:
    cmp rcx, 256
    jge prga_setup
    
    ; Calcul de j = (j + S[i] + key[i % 8]) & 0xFF
    movzx rax, byte [rsp + rcx] ; rax = S[i]
    add r10, rax                ; j += S[i]
    
    mov r11, rcx                ; r11 = i
    and r11, 7                  ; r11 = i % 8
    movzx r11, byte [rsp + 256 + r11] ; r11 = key[i % 8]
    add r10, r11                ; j += key[i % 8]
    and r10, 0xFF               ; j = j % 256
    
    ; Swap(S[i], S[j])
    movzx r11, byte [rsp + r10] ; r11 = S[j]
    mov byte [rsp + rcx], r11b  ; S[i] = S[j]
    mov byte [rsp + r10], al    ; S[j] = ancienne valeur de S[i] (dans al)
    
    inc rcx
    jmp ksa_loop

    ; d) PRGA : Déchiffrement du code
prga_setup:
    xor rcx, rcx                ; i = 0
    xor r10, r10                ; j = 0
    xor r9, r9                  ; compteur d'octets déchiffrés (n = 0)
prga_loop:
    cmp r9, r15                 ; Si on a traité toute la taille (r15)
    jge end_rc4
    
    ; i = (i + 1) & 0xFF
    inc rcx
    and rcx, 0xFF
    
    ; j = (j + S[i]) & 0xFF
    movzx rax, byte [rsp + rcx] ; rax = S[i]
    add r10, rax                ; j += S[i]
    and r10, 0xFF               ; j = j % 256
    
    ; Swap(S[i], S[j])
    movzx r11, byte [rsp + r10] ; r11 = S[j]
    mov byte [rsp + rcx], r11b  ; S[i] = S[j]
    mov byte [rsp + r10], al    ; S[j] = ancienne valeur S[i] (dans al)
    
    ; Keystream_byte = S[(S[i] + S[j]) & 0xFF]
    add rax, r11                ; rax = S[i] + S[j]
    and rax, 0xFF               ; index = (S[i] + S[j]) % 256
    movzx rax, byte [rsp + rax] ; rax = l'octet keystream
    
    ; XOR avec le code chiffré
    xor byte [r14 + r9], al     ; Code[n] ^= Keystream_byte
    
    inc r9
    jmp prga_loop

end_rc4:
    ; e) Nettoyage de la pile
    add rsp, 264                ; On libère la S-Box (256) + la Clé (8)

    ; ==========================================================

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