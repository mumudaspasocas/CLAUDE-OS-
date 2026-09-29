; ============================================================
; boot.asm — Ponto de entrada do nosso SO
; Escrito em NASM. Cria um cabeçalho Multiboot para que o GRUB
; consiga carregar nosso kernel, prepara a pilha e chama o
; kernel_main() que está escrito em C.
; ============================================================

; ---- Cabeçalho Multiboot (exigido pelo GRUB) ----
MBALIGN     equ  1 << 0            ; alinhar módulos em páginas de 4KB
MEMINFO     equ  1 << 1            ; pedir mapa de memória ao bootloader
FLAGS       equ  MBALIGN | MEMINFO
MAGIC       equ  0x1BADB002        ; número mágico que o GRUB procura
CHECKSUM    equ -(MAGIC + FLAGS)   ; magic + flags + checksum = 0

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; ---- Pilha (stack) do kernel ----
; A CPU precisa de uma pilha válida antes de executarmos qualquer
; código C. Reservamos 16KB.
section .bss
align 16
stack_bottom:
    resb 16384          ; 16 KB de espaço para a pilha
stack_top:

; ---- Ponto de entrada ----
section .text
global _start
extern kernel_main        ; função escrita em C (kernel.c)

_start:
    ; Configura o topo da pilha (a pilha cresce "para baixo")
    mov esp, stack_top

    ; (Opcional, mas recomendado) zera os flags de direção etc.
    cli                    ; desliga interrupções até configurarmos a IDT

    ; Chama nossa função em C — é aqui que a mágica acontece
    call kernel_main

    ; Se kernel_main() algum dia retornar, travamos a CPU
    ; num loop infinito para não "cair no vazio"
.hang:
    cli
    hlt
    jmp .hang
