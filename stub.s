[BITS 64]
    global _start

_start:
    ; Sauvegarde des registres
    push rax
    push rdi
    push rsi
    push rdx

    ; Affichage de "....WOODY...."
    mov rax, 1          ; syscall write
    mov rdi, 1          ; fd stdout
    lea rsi, [rel msg]  ; adresse relative (PIC)
    mov rdx, 14         ; taille
    syscall

    ; Restauration des registres
    pop rdx
    pop rsi
    pop rdi
    pop rax

    ; Sortie propre pour tester
    mov rax, 0xAAAAAAAA ; Valeur bidon à remplacer
    jmp rax

msg: db "....WOODY....", 10