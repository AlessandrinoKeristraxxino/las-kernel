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



//questo lo ha fatto claude penso vada bene
//ma 'sto build.rs a cosa servirebbe?
//anche perchè non abbiamo la libreria standard e quindi non si può usare println!()
//ma l'ha fatto claude quindi...