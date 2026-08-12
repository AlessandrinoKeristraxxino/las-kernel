// crate/src/ffi.rs

use core::ffi::{c_char, c_int, c_uint, c_void};

#[repr(C)]
#[derive(Debug, Clone, Copy)]

pub struct MultibootInfo {
    pub flags: u32,
    pub mem_upper: u32,
    pub mem_lower: u32,
}

/// C function declarations
unsafe extern "C" {
    // VGA
    pub fn vga_init();
    pub fn vga_putchar(c: u8);
    pub fn vga_write(s: *const c_char);
    pub fn vga_set_color(fg: u8, bg: u8);
    pub fn vga_clear();
    pub fn vga_scroll(uod: c_char);

    // Keyboard
    pub fn keyboard_init();
    pub fn keyboard_haschar() -> c_int;
    pub fn keyboard_getchar() -> u8;

    // Timer
    pub fn timer_init(frequency: u32);
    pub fn timer_get_ticks() -> u64;

    // IRQ
    pub fn irq_init();
    pub fn irq_enable();
    pub fn irq_disable();
}