// crate/src/main.rs

//! Kernel entry point
//! It gives to crate/src/boot.c the kernel_main() function

#![no_std]
#![no_main]

extern crate alloc;

mod ffi;
mod memory;

use core::panic::PanicInfo;
use alloc::boxed::Box;
use alloc::vec::Vec;

use ffi::MultibootInfo;
use memory::heap;

#[no_mangle]
pub extern "C" fn kernel_main(multiboot_info_addr: u64) -> ! {
    unsafe {
        /// DA QUI IN POI ALLOC FUNZIONA
        /// SI PUò USARE L'HEAP
        heap::init_heap();
        
        ffi::vga_init();
        ffi::keyboard_init();
        ffi::timer_init(100);
        ffi::irq_init();
        ffi::irq_enable();
    }

    let msg = "Kernel is working";
    unsafe { ffi::vga_write(msg.as_ptr() as *const _); }

    /// PUò FUNZIONARE
    let mut v = Vec::new();
    v.push(42);

    loop {}
}

#[repr(C)]
struct TagHeader {
    typ: u32,
    size: u32,
}

/// Reads the `total_size` field of the Multiboot2 structure at the given address
/// 
/// # Safety
/// The caller must ensure that `mb_info_addr` is a valid physical address
/// returned by a Multiboot2-compliant bootloader, yet
/// mapped and readable in the current address space.
unsafe fn multiboot_total_size(mb_info_addr: u64) -> u32 {
    let ptr = mb_info_addr as *const u32;
    ptr.read_volatile()
}

/// Panic Handler when the program crash
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
