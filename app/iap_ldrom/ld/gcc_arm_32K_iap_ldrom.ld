MEMORY
{
    IROM1 (rx) : ORIGIN = 0x0000F000, LENGTH = 0x1000    /* 4KB */
    IROM2 (rx) : ORIGIN = 0x00100000, LENGTH = 0x800     /* 2KB */
    RAM   (rw) : ORIGIN = 0x20000000, LENGTH = 0x1000    /* 4KB */
}

GROUP(libgcc.a libc.a libm.a libnosys.a)

ENTRY(Reset_Handler)

SECTIONS
{
    /* IROM2：手動標記放置大函式 / 少用函式 */
    .irom2_text :
    {
        __irom2_text_start = .;
        *(.irom2_text*)
        __irom2_text_end = .;
    } > IROM2 AT > IROM2

    /* IROM2：專用常數與指標陣列段 */
    .rodata_irom2 :
    {
        __rodata_irom2_start = .;
        KEEP(*(.rodata_irom2*))
        __rodata_irom2_end = .;
    } > IROM2 AT > IROM2

    /* IROM2：放 pointer 陣列 (uint8_t* / uint32_t*) */
    .rodata_ptr8 :
    {
        *(.rodata_ptr8*)
    } > IROM2 AT > IROM2

    /* 放 uint32_t 指標陣列 */
    .rodata_ptr32 :
    {
        *(.rodata_ptr32*)
    } > IROM2 AT > IROM2

    /* 主程式碼優先放 IROM1 */
    .text :
    {
        KEEP(*(.vectors))
        __Vectors_End = .;
        __Vectors_Size = __Vectors_End - __Vectors;
        __end__ = .;

        *(.text*)         /* 主要程式碼放 IROM1 */
        *(.rodata*)

        KEEP(*(.init))
        KEEP(*(.fini))

        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        KEEP(*(.eh_frame*))
    } > IROM1 AT > IROM1

    /* Exception unwinding tables */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > IROM1

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > IROM1
    __exidx_end = .;

    __etext = .;

    /* Data section */
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM

    /* BSS section */
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    /* Heap */
    .heap (COPY):
    {
        __HeapBase = .;
        __end__ = .;
        end = __end__;
        KEEP(*(.heap*))
        __HeapLimit = .;
    } > RAM

    /* Stack dummy */
    .stack_dummy (COPY):
    {
        KEEP(*(.stack*))
    } > RAM

    /* Stack settings */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
