// crate/src/main.rs

#![no_std]
#![no_main]

mod ffi;
mod rust_drivers;

use core::panic::PanicInfo;
use ffi::MultibootInfo;

#[no_mangle]
pub extern "C" fn _start() -> ! {
    unsafe {
        ffi::vga_init();
        ffi::keyboard_init();
        ffi::timer_init(100);
        ffi::irq_init();
        ffi::irq_enable();
    }

    let msg = "Kernel is working";
    unsafe { ffi::vga_write(msg.as_ptr() as *const _); }

    loop {}
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    if let Some(location) = info.location() {
        let _ = unsafe {
            ffi::vga_write(b"Panic at \0".as_ptr() as *const _);
        };
    }
    unsafe {
        ffi::vga_write(b"Kernel panicked\n\0".as_ptr() as *const _);
    }

    loop {}
}
