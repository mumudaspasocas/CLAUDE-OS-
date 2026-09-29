/* ============================================================
 * linker.ld — Diz ao linker COMO montar o binário final do kernel
 * O GRUB espera encontrar o kernel carregado a partir de 1MB
 * (0x100000), que é o endereço "seguro" acima da área reservada
 * do BIOS/hardware legado.
 * ============================================================ */

ENTRY(_start)

SECTIONS
{
    . = 1M;

    /* Primeiro o multiboot header, tem que estar bem no início */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Dados só de leitura */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Dados inicializados */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Dados não inicializados + pilha (bss) */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
