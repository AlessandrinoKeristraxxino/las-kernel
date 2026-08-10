# Piano di sviluppo: OS ibrido C + Rust

## Filosofia del progetto

Invece di scegliere un linguaggio unico, sfruttate i punti di forza di entrambi lungo un confine tecnico chiaro:

- **C**: dove serve controllo minuzioso su hardware/boot, dove esistono già tool consolidati (bootloader, ASM inline), dove la letteratura OS-dev è più abbondante.
- **Rust**: dove la sicurezza della memoria conta di più — gestione memoria, strutture dati del kernel, parsing, filesystem, driver "alto livello".

Il confine tra i due passa attraverso una **ABI in stile C** (`extern "C"`), che Rust supporta nativamente. Questo vi permette di lavorare in parallelo senza bloccarvi a vicenda.

---

## Architettura del confine C/Rust

```
┌─────────────────────────────────────┐
│  Bootloader (Assembly + C)           │  ← Alessandro (proposta)
│  - Modalità reale → protetta/long    │
│  - Setup GDT/IDT iniziale            │
│  - Carica il kernel in memoria       │
└─────────────────────────────────────┘
              │ salto a kernel_main()
              ▼
┌─────────────────────────────────────┐
│  Kernel core (Rust, #![no_std])      │  ← amico (proposta)
│  - Memory manager (paging, heap)     │
│  - Scheduler / processi              │
│  - Strutture dati kernel             │
└─────────────────────────────────────┘
              │ FFI extern "C"
              ▼
┌─────────────────────────────────────┐
│  Driver layer (C)                    │
│  - Driver hardware (VGA, tastiera,   │
│    disco, timer)                     │
│  - Interrupt handler grezzi          │
└─────────────────────────────────────┘
```

Le assegnazioni sopra sono solo un punto di partenza: decidete voi due chi prende cosa in base a chi si sente più a suo agio con Assembly/C a basso livello vs Rust `no_std`.

---

## Roadmap in 6 fasi

### Fase 0 — Setup e accordi (1 settimana)
- Scegliete target architetturale: **x86_64** consigliato (più documentazione/tool rispetto ad ARM per un primo OS)
- Repo Git condiviso con struttura chiara: `/boot`, `/kernel`, `/drivers`, `/docs`
- Toolchain: `nasm` o `gas` per assembly, `gcc`/`clang` cross-compiler, Rust target `x86_64-unknown-none`
- Definite **subito** l'ABI di confine (nomi funzioni, convenzioni di chiamata, struct condivise) — questo è il contratto che vi permette di lavorare separati
- Emulatore: **QEMU** per test rapidi, senza dover flashare hardware reale

### Fase 1 — Bootloader minimo (2-3 settimane)
- Boot da BIOS/UEFI (consiglio: usare **GRUB con Multiboot2** invece di scrivere un bootloader da zero — risparmia settimane)
- Passaggio da modalità reale a modalità protetta/long mode
- Setup GDT (Global Descriptor Table) minimale
- Salto al kernel con parametri Multiboot passati correttamente
- **Persona**: chi preferisce C/Assembly

### Fase 2 — Kernel entry e primitive base (3-4 settimane)
- `kernel_main()` in Rust che riceve i parametri dal bootloader via FFI
- IDT (Interrupt Descriptor Table) e gestione interrupt/eccezioni base
- Output testuale minimo (VGA text buffer o framebuffer) per debug
- Gestione panic in Rust `no_std` (serve un `panic_handler` custom)
- **Persona**: lavoro condiviso, è il punto di saldatura tra i due mondi

### Fase 3 — Memory management (4-6 settimane)
- Physical memory manager (bitmap o buddy allocator)
- Paging (page tables, virtual memory)
- Heap allocator per Rust (`GlobalAlloc` custom) — qui Rust inizia a "ripagare" l'investimento: una volta che l'allocator è solido, tutte le strutture dati sicure di Rust (Vec, Box, ecc.) diventano disponibili
- **Persona**: chi ha preso Rust, è il cuore della sicurezza memoria

### Fase 4 — Driver e I/O (4-6 settimane)
- Driver tastiera (interrupt-driven)
- Driver timer (PIT/APIC) per scheduling
- Driver disco base (ATA/AHCI) se volete filesystem
- Questi possono restare in C se preferite, comunicando col kernel Rust via FFI, oppure passarli a Rust mano a mano che vi sentite più sicuri
- **Persona**: chi preferisce C, o split per dispositivo

### Fase 5 — Scheduler e processi (4-6 settimane)
- Context switching (Assembly + wrapper Rust/C)
- Scheduler base (round robin è sufficiente per iniziare)
- Syscall interface minimale
- **Persona**: lavoro condiviso, richiede sincronizzazione stretta sull'ABI

### Fase 6 — Filesystem e userspace (tempo aperto)
- Filesystem minimale (anche solo in RAM, tipo initrd) prima di pensare a FAT32/ext2
- Caricamento ed esecuzione di programmi userspace
- Da qui in poi il progetto diventa aperto: shell, librerie, ecc.

---

## Divisione dei compiti — proposta di partenza

| Area | Responsabile suggerito | Linguaggio |
|---|---|---|
| Bootloader / Assembly | chi preferisce C | C + ASM |
| GDT/IDT setup | condiviso | C/ASM → chiamato da Rust |
| Memory manager | chi vuole Rust | Rust `no_std` |
| Heap allocator | chi vuole Rust | Rust |
| Driver tastiera/timer | chi preferisce C | C (o Rust se si convince) |
| Scheduler | condiviso | Rust core + ASM per context switch |
| Build system / CI | chi è più a suo agio con tooling | Makefile + cargo |

Rivedete questa tabella dopo la Fase 2: a quel punto avrete un'idea più concreta di dove ciascuno rende meglio.

---

## Rischi e nodi da sciogliere insieme

1. **Build system unico**: dovete far compilare insieme oggetti C e Rust e linkarli in un unico binario ELF (o immagine boot). Serve un Makefile/build script condiviso fin dall'inizio, non aggiunto dopo.
2. **Convenzioni di chiamata**: allineatevi su `extern "C"` ovunque nel confine, niente strutture dati Rust "fantasiose" (generics, trait objects) che attraversano l'FFI.
3. **Debug cross-linguaggio**: GDB funziona su entrambi ma il "context switch" mentale nel debug misto è reale — prevedete tempo extra qui.
4. **Evitare lo stallo da disaccordo**: se in futuro sorgeranno altri disaccordi sul chi-fa-cosa, datevi una regola semplice (es. chi ha scritto il primo prototipo di un modulo lo possiede, l'altro rivede in PR).

---

## Risorse utili per iniziare

- **OSDev Wiki** — riferimento principale per bootloader, paging, interrupt (linguaggio-agnostico, ottimo per la parte C/ASM)
- **"Writing an OS in Rust" (Philipp Oppermann, blog_os)** — guida passo-passo molto seguita per la parte Rust `no_std`
- **Redox OS** e **Theseus** — codice sorgente di OS reali scritti in Rust, utili come riferimento quando vi bloccate
- **"Operating Systems: Three Easy Pieces"** (libro gratuito online) — per la teoria (scheduler, memoria virtuale, filesystem) indipendente dal linguaggio

---

## Prossimo passo concreto

Prima riunione: decidete insieme
1. Chi parte con il bootloader (Fase 1)
2. Il formato esatto dei parametri passati da bootloader a kernel (struct condivisa)
3. Repo + branch strategy + primo commit con lo scheletro di build (anche vuoto, ma che compili e linki C+Rust insieme)

Se il primo "Hello World" cross-linguaggio (bootloader C che stampa a video e poi salta a un kernel_main Rust che stampa un'altra riga) funziona in QEMU, avete già validato tutta l'architettura di fondo.
