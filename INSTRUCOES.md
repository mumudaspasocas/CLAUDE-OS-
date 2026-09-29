# Como compilar e rodar o seu SO

## ⚠️ Aviso importante sobre o ambiente
Você mencionou que usa **Windows** com MASM/TASM. Para desenvolvimento de
sistema operacional "bare metal" como este, o ecossistema padrão é
Linux (mesmo que só para compilar) porque precisamos de:

- **NASM** (montador, não o MASM/TASM — sintaxe parecida, mas é o padrão do osdev)
- Um **cross-compiler** `i686-elf-gcc` (um GCC especial que gera código
  que não depende do Windows/Linux, só do hardware)
- `grub-mkrescue` + `xorriso` para gerar uma imagem `.iso` bootável
- `qemu-system-i386` para testar sem precisar de um PC/pendrive de verdade

A forma mais fácil no Windows é usar o **WSL2** (Windows Subsystem for Linux).

## Passo 1 — Instalar o WSL2
No PowerShell (como administrador):
```powershell
wsl --install
```
Reinicie o PC se pedido, e crie seu usuário Ubuntu.

## Passo 2 — Instalar as ferramentas (dentro do WSL/Ubuntu)
```bash
sudo apt update
sudo apt install -y build-essential nasm xorriso grub-pc-bin grub-common qemu-system-x86 curl
```

### Instalar o cross-compiler i686-elf-gcc
Não existe pacote apt pronto — o jeito mais simples é usar o script
automatizado do projeto OSDev:
```bash
mkdir -p ~/opt/cross && cd ~/opt/cross
curl -LO https://github.com/lordmilko/i686-elf-tools/releases/latest/download/i686-elf-tools-linux.zip
unzip i686-elf-tools-linux.zip
echo 'export PATH="$HOME/opt/cross/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
i686-elf-gcc --version   # deve mostrar a versão instalada
```

## Passo 3 — Copiar os arquivos
Copie estes 5 arquivos para uma pasta dentro do WSL (ex: `~/meuso/`):
- `boot.asm`
- `kernel.c`
- `linker.ld`
- `Makefile`
- `grub.cfg`

(No WSL, seus arquivos do Windows ficam acessíveis em `/mnt/c/...`,
mas é mais rápido copiar para dentro do Linux mesmo, ex: `~/meuso/`)

## Passo 4 — Compilar e rodar
```bash
cd ~/meuso
make        # compila tudo e gera meuso.iso
make run    # abre o QEMU e boota seu SO
```

Se tudo der certo, uma janela do QEMU vai abrir mostrando a tela azul
com o texto "Meu SO em Assembly + C - v0.1" 🎉

## O que cada arquivo faz
| Arquivo      | Função |
|--------------|--------|
| `boot.asm`   | Primeiro código executado. Declara o cabeçalho Multiboot (pra o GRUB reconhecer o kernel), prepara a pilha e chama o C |
| `kernel.c`   | O "sistema operacional" propriamente dito: escreve texto na tela via memória de vídeo VGA |
| `linker.ld`  | Diz onde cada parte do código deve ficar na memória final |
| `Makefile`   | Automatiza os comandos de compilação |
| `grub.cfg`   | Configuração do menu de boot do GRUB |

## Próximos passos (quando quiser evoluir o SO)
Peça pra eu adicionar, um de cada vez:
1. **GDT** (Global Descriptor Table) — segmentação de memória
2. **IDT + handlers de interrupção** — pra lidar com exceções da CPU
3. **Driver de teclado** — ler entrada via portas PS/2
4. **Paginação (paging)** — memória virtual
5. **Um shell simples** rodando dentro do próprio SO

Cada um desses é um degrau clássico do curso "OSDev" — dá pra ir
evoluindo esse mesmo projeto.
