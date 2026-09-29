# ============================================================
# Makefile — automatiza a montagem do SO
#
# Requer (instale via WSL/Linux — veja INSTRUCOES.md):
#   - nasm            (monta o boot.asm)
#   - i686-elf-gcc    (compilador C cross-compiler p/ bare metal)
#   - i686-elf-ld     (linker cross-compiler)
#   - grub-mkrescue + xorriso (monta o .iso bootável)
#   - qemu-system-i386 (roda o SO numa máquina virtual)
# ============================================================

CC      = i686-elf-gcc
LD      = i686-elf-ld
ASM     = nasm

CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -O2 -nostdlib

all: meuso.iso

boot.o: boot.asm
	$(ASM) -f elf32 boot.asm -o boot.o

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o $(CFLAGS)

meuso.bin: boot.o kernel.o linker.ld
	$(LD) $(LDFLAGS) -o meuso.bin boot.o kernel.o

meuso.iso: meuso.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp meuso.bin isodir/boot/meuso.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o meuso.iso isodir

run: meuso.iso
	qemu-system-i386 -cdrom meuso.iso

clean:
	rm -rf *.o *.bin *.iso isodir

.PHONY: all run clean
