// crate/src/ffi.rs

use core::ffi::{c_char, c_int, c_uint, c_void};

#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum KeyboardLayout {
    Generic = 0,
    Uk = 1,
    It = 2,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct MultibootInfo {
    pub flags: u32,
    pub mem_upper: u32,
    pub mem_lower: u32,
}

/// C function declarations
unsafe extern "C" {
    KeyboardLayout;

    // VGA
    pub fn vga_init();              
    pub fn vga_putchar(c: u8);
    pub fn vga_write(s: *const c_char);
    pub fn vga_writec(s: *const c_char, c: *const [u8; 2]);
    pub fn vga_set_color(fg: u8, bg: u8);
    pub fn vga_clear();
    pub fn vga_scroll(uod: c_char);

    // Keyboard
    pub fn keyboard_init();
    pub fn keyboard_getscncd() -> u8; // return 0 se non ci sono input ritorna lo scancode
    pub fn keyboard_haschar(c: *const c_char) -> u8; 
    puf fn keyboard_set_layout(layout: KeyboardLayout);

    // Timer
    pub fn timer_init(frequency: u32); // min freq 20hz
    pub fn timer_get_ticks() -> u64;

    // IRQ
    pub fn irq_init();
    pub fn irq_enable();
    pub fn irq_disable();
}