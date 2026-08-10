// crate/src/main.rs

#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[derive(Copy, Clone)]
#[repr(u8)]
enum Colors {
    White = 0x0f,
}

#[no_mangle]
pub extern "C" fn _start() -> ! {
    let vga_buffer = 0xb8000 as *mut u8;

    unsafe {
        vga_buffer.offset(0).write_volatile(b'H');
        vga_buffer.offset(1).write_volatile(Colors::White as u8);
    }

    loop {}
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
