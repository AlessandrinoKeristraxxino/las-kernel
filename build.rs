// fn main() {
//     cc::Build::new()
//         .file("src/boot.c")
//         .file("src/vga.c")
//         .file("src/keyboard.c")
//         .file("src/timer.c")
//         .file("src/irq.c")
//         .compile("kernel_c");
    
//     // Usa il linker script
//     println!("cargo:rustc-link-arg=-Tlinker.ld");
// }



//questo lo ha fatto claudio penso vada bene