;
; Implementation minimale d'un spinlock
;
SECTION .data

SECTION .text
global mini_spinlock_lock
global mini_spinlock_unlock

mini_spinlock_lock:          ; adresse du verrou dans rdi
    mov rcx, 1               ; valeur du verrou occupe
mini_spinlock_retry:
    xor rax, rax             ; remettre a zero rax
    lock cmpxchg [rdi], rcx  ; compare la valeur [rdi] avec rax
                             ; si ([rdi] == rax) i.e. verrou occupe
                             ;   ZF = 1, [rdi] = rcx
                             ; sinon
                             ;   ZF = 0, rax = [rdi]
    jnz mini_spinlock_retry ; si (ZF == 0), recommence
    ret                      ; verrou obtenu

mini_spinlock_unlock:        ; adresse du verrou dans rdi
    mov qword [rdi], 0       ; libere le verrou
    ret
