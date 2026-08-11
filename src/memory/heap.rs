// crate/src/memory/mod.rs

use core::alloc::{GlobalAlloc, Layout};
use core::ptr;
use spin::Mutex;
use x86_64::align_up;

pub const HEAP_START: usize = 0x4444_4444_0000;
pub const HEAP_SIZE: usize = 100 * 1024;

#[global_allocator]
pub static ALLOCATOR: Locked<BumpAllocator> = Locked::new(BumpAllocator::new());

pub struct BumpAllocator {
    heap_start: usize,
    heap_end: usize,
    next: usize,
    allocations: usize,
}

pub struct Locked<A> {
    inner: Mutex<A>,
}

impl BumpAllocator {
    pub const fn new() -> Self {
        BumpAllocator {
            heap_start: 0,
            heap_end: 0,
            next: 0,
            allocations: 0,
        }
    }

    pub unsafe fn init(&mut self, heap_start: usize, heap_size: usize) {
        self.heap_start = heap_start;
        self.heap_start + heap_size;
        self.next = heap_start;
    }
}

impl<A> Locked<A> {
    pub const fn new(inner: A) -> Self {
        Locked { inner: Mutex::new(inner) }
    }

    pub fn lock(&self) -> spin::MutexGuard<A> {
        self.inner.lock()
    }
}

unsafe impl GlobalAlloc for Locked<BumpAllocator> {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let mut bump = self.lock();
        
        let alloc_start = align_up(bump.next, layout.align());
        let alloc_end = match alloc_start.checked_add(layout.size()) {
            Some(end) => end,
            None => return ptr::null_mut(),
        };

        if alloc_end <= bump.heap_end {
            bump.next = alloc_end;
            bump.allocations += 1;
            alloc_start as *mut u8
        } else {
            ptr::null_mut()
        }
    }

    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {
        let mut bump = self.lock();
        bump.allocations -= 1;
        
        if bump.allocations == 0 {
            bump.next = bump.heap_start;
        }
    }
}

pub unsafe fn init_heap() {
    ALLOCATOR.lock().init(HEAP_START, HEAP_SIZE);
}

/// Utility function to align an address to the top
fn align_up(addr: usize, align: usize) -> usize {
    (addr + align - 1) & !(align - 1)
}