MEMORY
{
    /* Define each memory region */
    CM55M_S_APP_ROM (rx)   : ORIGIN = 0x10000000, LENGTH = 0x10040000 - 0x10000000  // 256  KB
    CM55M_S_APP_DATA (rwx) : ORIGIN = 0x30000000, LENGTH = 0x30040000 - 0x30000000  // 256  KB
    CM55M_S_SRAM (rwx)     : ORIGIN = 0x34000000, LENGTH = 0x34054000 - 0x34000000  // 336  KB, please remember to check configTOTAL_HEAP_SIZE
    CM55M_S_EL_ALLOC (rw)  : ORIGIN = 0x34054000, LENGTH = 0x3416A000 - 0x34054000  // 1112 KB
    // CM55M_S_EL_FBUF (rw)   : ORIGIN = 0x3418F800, LENGTH = 0x34200000 - 0x3416A000  // 600  KB
    // CM55S_S_EL_HWENC (rw)  : ORIGIN = 0x36000000, LENGTH = 0x36060000 - 0x36000000
}

__HEAP_SIZE  = 0x4000;  // 16 KB
__STACK_SIZE = 0x4000;  // 16 KB

ENTRY(Reset_Handler)

SECTIONS
{
    /* MAIN TEXT SECTION */
    .table :
    {
        . = ALIGN(8);
        __vectors_start__ = ABSOLUTE(.);
        KEEP(*(.vectors))

        . = ALIGN(8);
        __privileged_functions_start__ = .;
        *(privileged_functions)
        *(privileged_functions*)
        __privileged_functions_end__ = .;

        . = ALIGN(8);
        __syscalls_flash_start__ = .;
        *(freertos_system_calls)
        *(freertos_system_calls*)
        __syscalls_flash_end__ = .;

        . = ALIGN(8);
        __unprivileged_flash_start__ = .;

    } > CM55M_S_APP_ROM

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD) :
    {
        . = ALIGN(8);
        _noinit = .;
        PROVIDE(__start_noinit_RAM = .) ;
        PROVIDE(__start_noinit_SRAM = .) ;

        *(.noinit .noinit.* .gnu.linkonce.n.*)

        . = ALIGN(4);
        _end_noinit = .;
        PROVIDE(__end_noinit_RAM = .) ;
        PROVIDE(__end_noinit_SRAM = .) ;

    } > CM55M_S_APP_ROM

    .ext_text :
    {
        . = ALIGN(4);
        *libfatfs.*:*(.text* .rodata*)

        . = ALIGN(4);
        *lib_i2c_comm.*:*(.text* .rodata*)

        . = ALIGN(4);
        *lib_img_proc.*:*(.text* .rodata*)

        . = ALIGN(4);
        *lib_spi_eeprom.*:*(.text* .rodata*)

        . = ALIGN(4);
        *lib_spi_psram.*:*(.text* .rodata*)

        . = ALIGN(4);
        *lib_spi_ptl.*:*(.text* .rodata*)

        . = ALIGN(4);
        *libaudio.*:*(.text* .rodata*)

        . = ALIGN(4);
        *libsscma_micro.*:*(.text* .rodata*)
        
        . = ALIGN(4);
        *libtflmtag2209_u55tag2205_gnu.*:*(.text* .rodata*)

    } > CM55M_S_APP_DATA

    .text :
    {
        . = ALIGN(8);
        *(.text*)

        . = ALIGN(8);
        KEEP(*(.init))

        /* preinit data */
        . = ALIGN(8);
        PROVIDE_HIDDEN(__preinit_array_start = .);
        _spreinit_array = .;
        KEEP(*(.preinit_array))
        KEEP(*(.preinit_array*))
        _epreinit_array = .;
        PROVIDE_HIDDEN(__preinit_array_end = .);

        . = ALIGN(8);
        _sinit_array = .;
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(.init_array))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
        _einit_array = .;

        /* .ctors */
        . = ALIGN(8);
        KEEP(*crtbegin.o(.ctors))
        KEEP(*crtbegin?.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))

        /* .dtors */
        . = ALIGN(8);
        KEEP(*crtbegin.o(.dtors))
        KEEP(*crtbegin?.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))

        . = ALIGN(8);
        KEEP(*(.fini));

        . = ALIGN(8);
        _sfini_array = .;
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(.fini_array))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
        _efini_array = .;

        . = ALIGN(8);
        KEEP(*(.eh_frame*))

    } > CM55M_S_APP_ROM

    .rodata :
    {
        FILL(0x0000)
        . = ALIGN(4);
        __rodata_start = .;

        *(.rodata .rodata.* .constdata .constdata.*)

        __rodata_end = .;

    } > CM55M_S_APP_DATA

    .persistent :
    {
        FILL(0x0000)
        . = ALIGN(4);
        PROVIDE (__persistent_start = .);

        *(.persistent .persistent.* .gnu.linkonce.p.*)

        PROVIDE (__persistent_end = .);

    } > CM55M_S_APP_DATA

    .elHeap :
    {
        . = ALIGN(32);
        KEEP(*(.bss.*elHeap*))
    } > CM55M_S_EL_ALLOC

    .ucHeap :
    {
        . = ALIGN(8);
        *(.bss.ucHeap)
    } > CM55M_S_SRAM

    .ARM.extab :
    {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > CM55M_S_APP_ROM

    .ARM.exidx :
    {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > CM55M_S_APP_ROM

    .copy.table :
    {
        . = ALIGN(8);
        __copy_table_start__ = .;

        LONG(LOADADDR(.data));
        LONG(ADDR(.data));
        LONG(SIZEOF(.data) / 4);

        /* Add each additional data section here */
        __copy_table_end__ = .;

    } > CM55M_S_APP_ROM

    .zero.table :
    {
        . = ALIGN(8);
        __zero_table_start__ = .;

        /* Add each additional bss section here */
        LONG(ADDR(.bss));
        LONG(SIZEOF(.bss) / 4);

        __zero_table_end__ = .;
    } > CM55M_S_APP_ROM

    . = ALIGN(32);
    __unprivileged_flash_end__ = .;

    .jcr : {
        . = ALIGN(8);
        KEEP (*(.jcr))
    } > CM55M_S_APP_DATA

    /* Main DATA section (BOOTROM_SRAM) */
    .data :
    {
        FILL(0x0000)
        . = ALIGN(8);
        __data_start__ = .;
        __privileged_sram_start__ = .;

        *(privileged_data)
        *(privileged_data*)

        . = ALIGN(8);
        __privileged_sram_end__ = .;

        . = ALIGN(8);
        __data_start = .;

        *(vtable)
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
        *(.data1)

        /* All data end */
        . = ALIGN(8);
        __data_end__ = .;

    } > CM55M_S_APP_DATA

    .bss :
    {
        . = ALIGN(8);
        __bss_start__ = .;

        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)

        . = ALIGN(8);
        __bss_end__ = .;

    } > CM55M_S_APP_DATA

    /* Reserve and place Heap within memory map */
    .heap (COPY) :
    {
        . = ALIGN(8);
        __HeapBase = .;
        PROVIDE(__HeapBase = .);
        end = __HeapBase;

        FILL(0xDEAD)
        . = . + __HEAP_SIZE;

        . = ALIGN(8);
        __HeapLimit = .;
        PROVIDE(__HeapLimit = .);

    } > CM55M_S_APP_DATA

    /* Locate actual Stack in memory map */
    .stack (ORIGIN(CM55M_S_APP_DATA) + LENGTH(CM55M_S_APP_DATA) - __STACK_SIZE - 32) (COPY) :
    {
        . = ALIGN(8);
        __StackLimit = .;
        PROVIDE(__StackLimit = .);

        FILL(0x0000)
        . = . + __STACK_SIZE;

        . = ALIGN(8);
        __StackTop = .;
        PROVIDE(__StackTop = .);

    } > CM55M_S_APP_DATA

    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "heap overflowed with stack")

    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}