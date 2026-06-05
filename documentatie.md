# Poggi OS — Raport Tehnic de Arhitectură (Faza 1)

**Autor:** Vasilache  
**Versiune Sistem:** v0.7 (Hosted Mode)  
**Sistem de Operare Gazdă:** Debian 13

---

## 1. Introducere și Filozofia Proiectului

Poggi OS este un sistem de operare demonstrativ, minimalist, dezvoltat independent de biblioteca standard de C a Linux (`libc`). În Faza 1, kernelul rulează în mod **Hosted**, utilizând un strat de abstractizare hardware (HAL) pentru a simula interacțiunea cu ecranul și tastatura, permițând testarea algoritmilor de bază în siguranță.

---

## 2. Arhitectura Subsistemelor

### A. Stratul de Abstractizare Hardware (HAL)

Fișierul `poggi_hal_hosted.c` izolează apelurile de sistem native Linux. Kernelul Poggi OS nu folosește `printf` sau `malloc` direct în codul său de bază (`core/`), ci apelează exclusiv funcții din interfața HAL:

- `hal_print_string` / `hal_print_char`
- `hal_get_uptime` / `hal_get_total_memory`

### B. Gestionarea Memoriei (Poggi MemAlloc)

Sistemul implementează un **Bump Allocator** rapid peste un Heap static intern de 128 KB (`poggi_heap`). Alocările sunt aliniate la 4 bytes pentru eficiența procesorului.

- **Funcție principală:** `void* poggi_alloc(int size);`

### C. Sistemul de Fișiere Virtual (PoggiFS)

Un sistem de fișiere virtual, stocat integral în memoria RAM, care suportă indexarea a maximum 10 fișiere simultan. Fiecare fișier are o dimensiune fixă limitată la 256 bytes pentru stocarea structurată a textului.

- **Comenzi mapate:** `ls`, `cat`, `write`.

### D. Multitasking Cooperativ (Scheduler Round-Robin)

Planificatorul mută execuția între Task Control Blocks (TCB) în mod cooperativ. Task-urile rulează secvențial și cedează voluntar procesorul prin apelul funcției speciale:

- `void poggi_yield(void);`

### E. Comunicarea între Procese (Poggi IPC)

Pentru a evita partajarea nesigură a memoriei, kernelul pune la dispoziție o cutie poștală (`PoggiMailbox`). Procesele folosesc apeluri de sistem controlate pentru a trimite și recepționa mesaje întregi:

- `poggi_ipc_send(int value);`
- `poggi_ipc_receive(int* out_value);`

---

## 3. Structura Proiectului (Arborele de Fișiere)

```text
poggi_os/
├── Makefile
├── documentatie.md
├── include/
│   ├── poggi_fs.h
│   ├── poggi_hal.h
│   ├── poggi_ipc.h
│   ├── poggi_mem.h
│   ├── poggi_string.h
│   └── poggi_task.h
└── core/
    ├── main.c
    ├── poggi_fs.c
    ├── poggi_ipc.c
    ├── poggi_mem.c
    ├── poggi_string.c
    └── poggi_task.c
```
