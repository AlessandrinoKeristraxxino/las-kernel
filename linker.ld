ENTRY(kernel_entry)

SECTIONS {
    . = 0x100000;
    
    .text : {
        *(.text)
        *(.text.*)
    }
    
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
        *(.data.rel.ro)
        *(.data.rel.ro.*)
    }
    
    .bss : {
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(16);
        _bss_end = .;
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.debug_*)
        *(.note.GNU-stack)
        *(.gnu_debuglink)
        *(.gnu_debugdata)
    }
}