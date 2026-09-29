/* ============================================================
 * kernel.c — Kernel do nosso SO
 * Aqui a gente já está em "modo C". Sem biblioteca padrão (libc)
 * disponível, então tudo é feito na unha: escrever direto na
 * memória de vídeo em modo texto VGA (endereço 0xB8000).
 * ============================================================ */

#include <stdint.h>
#include <stddef.h>

/* Modo texto VGA: 80 colunas x 25 linhas.
 * Cada caractere na tela ocupa 2 bytes: [caractere][atributo/cor] */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)

/* Cores disponíveis no modo texto VGA (4 bits) */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

/* Estado global do "terminal" simples */
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static size_t str_len(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;

    /* Limpa a tela inteira */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

static void terminal_scroll(void) {
    /* Move todas as linhas uma posição para cima */
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    /* Limpa a última linha */
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_scroll();
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_scroll();
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, str_len(data));
}

/* ============================================================
 * kernel_main — chamada pelo boot.asm assim que a pilha está
 * pronta. É o "main()" do nosso sistema operacional.
 * ============================================================ */
void kernel_main(void) {
    terminal_initialize();

    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
    terminal_writestring("========================================\n");
    terminal_writestring("   Meu SO em Assembly + C - v0.1        \n");
    terminal_writestring("========================================\n\n");

    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("Kernel carregado com sucesso!\n");
    terminal_writestring("Estamos rodando em modo protegido (32 bits).\n\n");

    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("Proximos passos possiveis:\n");
    terminal_writestring(" - GDT (Global Descriptor Table)\n");
    terminal_writestring(" - IDT (Interrupt Descriptor Table)\n");
    terminal_writestring(" - Driver de teclado (via portas PIC/PS2)\n");
    terminal_writestring(" - Gerenciamento de memoria (paging)\n");

    /* O kernel termina aqui, mas boot.asm entra em loop infinito
     * (hlt) para nao deixar a CPU "correndo solta". */
}
