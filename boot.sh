#!/bin/bash

# Curățăm complet fișierele din compilările anterioare pentru a evita conflictele
rm -f boot.bin kernel.bin kernel.o vga.o keyboard.o heap.o ata.o task.o task_switch.o temp_image.bin os_image.bin
rm -f kernel/entry.o
find . -type f \( -name "*.o" -o -name "*.bin" \) -delete

# 1. Asamblăm bootloader-ul de MBR (Sectorul 1)
nasm -f bin boot/boot.asm -o boot.bin

# 2. Asamblăm trambulina de Kernel în format ELF 64-bit
nasm -f elf64 kernel/entry.asm -o kernel/entry.o

# 2b. Asamblăm mecanismul hardware de schimbare context pentru Multitasking
nasm -f elf64 kernel/task_switch.asm -o task_switch.o

# 3. Compilăm kernelul, driverele și planificatorul de procese în format freestanding strict
gcc -m64 -ffreestanding -mno-red-zone -mno-sse -c kernel/kernel.c -o kernel.o
gcc -m64 -ffreestanding -mno-red-zone -mno-sse -c kernel/task.c -o task.o
gcc -m64 -ffreestanding -mno-red-zone -mno-sse -c drivers/vga.c -o vga.o
gcc -m64 -ffreestanding -mno-red-zone -mno-sse -c drivers/keyboard.c -o keyboard.o
gcc -m64 -ffreestanding -mno-red-zone -mno-sse -c drivers/heap.c -o heap.o
gcc -m64 -ffreestanding -mno-red-zone -mno-sse -c drivers/ata.c -o ata.o

# 4. Link-edităm toate modulele împreună conform regulilor stricte din linker.ld
ld -m elf_x86_64 -T kernel/linker.ld -o kernel.bin kernel/entry.o kernel.o vga.o keyboard.o heap.o ata.o task_switch.o task.o

# 5. Lipim bootloader-ul și kernelul compactat
cat boot.bin kernel.bin > temp_image.bin

# 6. CORECȚIE ALINIERE: Aliniem dinamic imaginea la o dimensiune sigură care acoperă kernelul mărit 
truncate -s 27648 temp_image.bin
cp temp_image.bin os_image.bin

# -----------------------------------------------------------------
# GENERARE ȘI INJECTARE STRUCTURĂ DE DATA PENTRU POGGIFS (HARD DISK)
# -----------------------------------------------------------------
truncate -s 40960 os_image.bin

# Creăm conținutul pentru fișierul text inițial de test
echo "Salutare! Acest text este citit direct dintr-un sector brut de pe Hard Disk de catre noul driver PoggiFS! Succes in OS Dev!" > nota.txt

# Injectăm conținutul din nota.txt direct în SECTORUL 56 al discului virtual
dd if=nota.txt of=os_image.bin bs=512 seek=56 conv=notrunc 2>/dev/null

# Generăm în mod binar freestanding primul slot din tabela de index a PoggiFS
python3 -c '
import struct
nume = b"nota.txt".ljust(16, b"\x00")
start_lba = struct.pack("<I", 56)
dimensiune = struct.pack("<I", 123)
padding = b"\x00" * 8

entry = nume + start_lba + dimensiune + padding
sector_complet = entry + (b"\x00" * (512 - len(entry)))

with open("index.bin", "wb") as f:
    f.write(sector_complet)
'

# Injectăm sectorul de index structurat root direct în SECTORUL 55 al discului virtual
dd if=index.bin of=os_image.bin bs=512 seek=55 conv=notrunc 2>/dev/null

# Curățăm fișierele temporare
rm -f index.bin nota.txt temp_image.bin

# 7. Rulăm în emulatorul QEMU
echo "Se lanseaza QEMU cu suportul extins PoggiFS si Multitasking Cooperativ..."
qemu-system-x86_64 -cpu qemu64 -drive format=raw,file=os_image.bin
