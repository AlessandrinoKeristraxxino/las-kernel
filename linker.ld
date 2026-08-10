ENTRY(_start)

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
        *(.bss)
        . = ALIGN(16);
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.debug_*)
    }
}