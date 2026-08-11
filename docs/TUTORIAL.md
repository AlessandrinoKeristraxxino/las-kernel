# LampScript Kernel - Tutorial di continuità

Questo documento spiega in modo teorico e pratico come continuare lo sviluppo del kernel `las-kernel`. L'obiettivo è fornire una panoramica completa dell'architettura esistente, dei meccanismi di build e dei prossimi passi logici per evolvere il progetto.

## 1. Panoramica generale del progetto

`las-kernel` è un kernel scritto in Rust e C per un sistema operativo minimale denominato LampScript OS. La base del progetto è una catena di avvio semplice che passa il controllo da C (`boot.c`) a Rust (`main.rs`).

Componenti principali:

- `src/boot.c`: punto di ingresso iniziale in C, azzera la sezione `.bss` e chiama `kernel_main()`
- `src/main.rs`: entry point del kernel Rust in `#![no_std]` e `#![no_main]`
- `src/ffi.rs`: dichiarazioni FFI verso funzioni C per VGA, tastiera, timer e IRQ
- `src/memory/heap.rs`: allocator bump per consentire l'uso di heap e `alloc`
- `src/drivers/*.c`: driver C per VGA, keyboard, timer e IRQ
- `linker.ld`: script di collegamento con layout di memoria definito
- `rust-toolchain.toml`: toolchain nightly e target `x86_64-unknown-none`
- `iso_root/limine.cfg`: configurazione del bootloader Limine per avviare il kernel

## 2. Toolchain e ambiente di build

Il progetto utilizza `cargo` con toolchain Rust nightly.

Predisposizione:

- installare Rust nightly
- aggiungere il target `x86_64-unknown-none`
- avere un linker compatibile come `ld` o `lld`
- installare `cc` se necessario per compilare i file C

Costruzione tipica:

```powershell
rustup override set nightly
rustup target add x86_64-unknown-none
cargo build
```

Nota: il `build.rs` compila i file C con `cc::Build` e passa l'argomento `-Tlinker.ld` al linker.

## 3. Flusso di avvio del kernel

### 3.1 `src/boot.c`

`boot.c` definisce la funzione `kernel_entry()`, che viene impostata come entry point nel linker script. Le responsabilità sono:

- azzerare la sezione `.bss`
- chiamare `kernel_main()`
- entrare in un ciclo `hlt` se `kernel_main()` ritorna

### 3.2 `linker.ld`

Lo script di link definisce l'indirizzo di caricamento a `0x100000` e posiziona le sezioni `.text`, `.rodata`, `.data` e `.bss`. Questo è un layout minimale tipico per un kernel che si avvia in modalità protetta/long mode.

### 3.3 `src/main.rs`

`kernel_main` è il punto di ingresso Rust esportato con `#[no_mangle] pub extern "C" fn kernel_main(...) -> !`.

Cosa fa ora:

- inizializza l'heap con `heap::init_heap()`
- avvia driver C: VGA, tastiera, timer, IRQ
- abilita le interruzioni
- stampa un messaggio su schermo
- alloca un `Vec` usando l'heap
- entra in un ciclo infinito

Questo è il punto da cui far crescere il kernel.

## 4. Interazione Rust/C (FFI)

Il file `src/ffi.rs` dichiara le funzioni C come `extern "C"` e definisce `MultibootInfo` come struttura C compatibile.

Le regole chiave:

- usare `#[repr(C)]` per i tipi condivisi fra C e Rust
- mantenere le chiamate `unsafe` attorno alle funzioni FFI
- usare `pub fn` solo per funzioni definite in C che verranno chiamate da Rust

In futuro, è utile estendere `ffi.rs` con funzioni di basso livello per leggere la tastiera, gestire il timer e controllare le interruzioni in modo più idiomatico.

## 5. Il driver VGA

`src/drivers/vga.c` fornisce un backend VGA molto semplice:

- `vga_init()` è un noop al momento
- `vga_putchar()` scrive direttamente al buffer VGA `0xB8000`
- `vga_write()` stampa stringhe terminate da `\0`

Aree di miglioramento:

- aggiungere gestione cursore e scrolling
- supportare colori con `vga_set_color`
- implementare `vga_clear`
- definire un wrapper Rust più sicuro per la scrittura su VGA

## 6. Gestione memoria e heap

Il file `src/memory/heap.rs` contiene un allocator bump semplice.

Funzionamento:

- `HEAP_START` è `0x4444_4444_0000`
- `HEAP_SIZE` è `100 KiB`
- `BumpAllocator::init()` imposta i limiti dell'heap
- `alloc()` allinea l'indirizzo e restituisce il blocco se c'è spazio
- `dealloc()` decrementa il contatore di allocazioni e resetta il bump quando tutte le allocazioni sono liberate

Da completare:

- correggere la riga `self.heap_start + heap_size;` in `self.heap_end = heap_start + heap_size;`
- gestire la memoria fisica e virtuale in modo coerente con l'MMU
- progettare allocator multipli (bump + linked list + slab)
- integrare la gestione del frame allocator e mappatura delle pagine

## 7. Interrupt e timer

Attualmente, i driver `timer.c`, `keyboard.c` e `irq.c` sono creati come stub vuoti. Questi moduli sono il cuore dell'interattività del kernel.

Cosa aspettarsi:

- `irq_init()` deve configurare la PIC/APIC e la tabella IDT
- `irq_enable()` e `irq_disable()` abilitano/disabilitano le IRQ
- `timer_init(frequency)` deve programmare il PIT o APIC timer
- il kernel deve installare un gestore di interrupt per il timer e la tastiera

## 8. Multiboot e bootloader

Il progetto dichiara la dipendenza `multiboot2`, ma al momento non usa i tag multiboot. È importante capire che il bootloader Limine fornisce informazioni multiboot al kernel.

Punti da sviluppare:

- leggere la struttura `MultibootInfo` passata da Limine
- interpretare la memoria disponibile, moduli e command line
- mappare le regioni fisiche e virtuali in base ai tag multiboot
- aggiungere supporto per l'uso della memoria `multiboot_info_addr`

## 9. Struttura di sviluppo consigliata

### 9.1 Obiettivi immediati

1. Riparare l'heap allocator
2. Implementare e testare `vga_clear()` e `vga_set_color()`
3. Applicare una mappa base della memoria e collegare il kernel all'heap
4. Evitare l'uso non necessario di `unsafe` in Rust, isolando le FFI in moduli piccoli

### 9.2 Funzionalità successive

1. Input da tastiera
2. Interrupt timer e contatore di tick
3. Gestione semplice dei processi o task
4. Sistema di chiamate di output/logging su VGA
5. Gestione del paging e modalità protetta/long mode

### 9.3 Architettura consigliata

- separare il codice hardware-specific in moduli distinti (`arch`, `drivers`, `memory`)
- mantenere il kernel Rust come layer superiore, con C usato solo per il codice molto basso livello e i bootstraps
- creare wrapper Rust attorno ai driver C per ridurre l'uso diretto di `unsafe`
- usare tipi `repr(C)` solo dove serve per l'interfaccia

## 10. Come organizzare la continuazione del lavoro

### 10.1 Documentare ogni passo

- aggiungi commenti nel codice e nel tutorial
- mantieni la documentazione aggiornata in `docs/`
- descrivi le scelte di design nel README o in `docs/INTERFACE.md`

### 10.2 Controlli di qualità

- usa `cargo build` regolarmente
- se possibile, configuralo per target `x86_64-unknown-none`
- aggiungi test Rust per i moduli che non dipendono dall'hardware
- usa `rustfmt` per uniformare lo stile

### 10.3 Esperimenti e prototipi

- prima di modificare il bootstrap, prova a isolare nuovi driver in moduli separati
- scrivi prototipi per `vga_write()` e per la stampa di numeri
- crea un sistema minimo di gestione della memoria fisica

## 11. Esempi pratici per il prossimo sviluppo

### A. Migliorare l'heap

1. correzione: `self.heap_end = heap_start + heap_size;`
2. aggiungere debug sulle dimensioni allocabili
3. creare un'interfaccia per `init_heap()` e `alloc` in Rust
4. migliorare la gestione dei caratteri infondo allo schermo in `vga_putchar()`

### B. Implementare la tastiera

1. leggere i codici di scan dal controller PS/2
2. convertire in caratteri ASCII
3. memorizzare in un buffer circolare
4. esporre funzioni Rust per il polling e la lettura

### C. Configurare la gestione delle IRQ

1. reindirizzare i PIC se si usa legacy PIC
2. installare la IDT
3. scrivere gestori per IRQ 0 (timer) e IRQ 1 (keyboard)
4. usare `cli`/`sti` in modo controllato

## 12. Conclusioni

Questo kernel è un punto di partenza solido per costruire un sistema operativo minimo. Il prossimo passo è completare le basi hardware: memoria, output video, input e interrupt.

Una volta stabilita una base stabile, si può passare al design di un modello di processo, driver di storage e un file system.

Buon lavoro su LampScript Kernel!
