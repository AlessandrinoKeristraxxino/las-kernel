ENTRY(kernel_entry)

SECTIONS {
    . = 0x100000;
    
    .text : {
        *(.text)
    }
    
    .rodata : {
        *(.rodata)
    }
    
    .data : {
        *(.data)
    }
    
    .bss : {
        _bss_start = .;
        *(.bss)
        . = ALIGN(16);
        _bss_end = .;
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.debug_*)
    }
}