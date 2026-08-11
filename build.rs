// fn main() {
//     cc::Build::new()
//         .file("src/boot.c")
//         .file("src/drivers/vga.c")
//         .file("src/drivers/keyboard.c")
//         .file("src/drivers/timer.c")
//         .file("src/drivers/irq.c")
//         .compile("kernel_c");
    
//     // Usa il linker script
//     println!("cargo:rustc-link-arg=-Tlinker.ld");
// }



//questo lo ha fatto claudio penso vada bene