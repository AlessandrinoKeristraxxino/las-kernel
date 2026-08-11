// crate/build.rs

/// Used for the build
/// Uitlity
fn main() {
    cc::Build::new()
        .file("src/boot.c")
        .file("src/drivers/vga.c")
        .file("src/drivers/keyboard.c")
        .file("src/drivers/timer.c")
        .file("src/drivers/irq.c")
        .flag("-ffreestanding")
        .flag("-fno-stack-protector")
        .flag("-nostdlib")
        .compile("kernel_c");
    
    println!("cargo:rustc-link-arg=-Tlinker.ld");
}
