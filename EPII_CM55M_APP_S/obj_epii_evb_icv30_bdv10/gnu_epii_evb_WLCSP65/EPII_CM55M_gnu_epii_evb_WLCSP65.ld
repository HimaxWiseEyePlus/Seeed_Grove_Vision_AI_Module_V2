MEMORY
{
  CM55M_S_APP_ROM (rx) : ORIGIN = 0x10000000, LENGTH = 0x40000
  CM55M_S_APP_DATA (rwx) : ORIGIN = 0x30000000, LENGTH = 0x40000
  CM55M_S_SRAM (rwx) : ORIGIN = ((0x34001000 +0x00004000)+0x0001A000), LENGTH = 0x00200000-(((0x34001000 +0x00004000)+0x0001A000)-(0x34000000))
}
__HEAP_SIZE = 0x10000;
__STACK_SIZE = 0x10000;
ENTRY(Reset_Handler)
SECTIONS
{
    .table : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.vectors))
        *(.after_vectors*)
        . = ALIGN(32);
        __privileged_functions_start__ = .;
        *(privileged_functions)
        *(privileged_functions*)
        . = ALIGN(32);
        __privileged_functions_end__ = (. - 1);
        . = ALIGN(32);
        __syscalls_flash_start__ = .;
        *(freertos_system_calls)
        *(freertos_system_calls*)
        . = ALIGN(32);
        __syscalls_flash_end__ = (. - 1);
        __unprivileged_flash_start__ = .;
    } > CM55M_S_APP_ROM
    .text_2 : ALIGN(4)
    {
        *libdriver.*:*(.text* .rodata*)
        *cvapp.o (.text* .rodata*)
    } > CM55M_S_SRAM
    .text : ALIGN(4)
    {
       *(.text*)
       KEEP(*freertos*/tasks.o(.rodata*))
       . = ALIGN(4);
       KEEP(*(.init))
       KEEP(*(.fini));
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
        . = ALIGN(4);
        KEEP(*(.eh_frame*))
    } > CM55M_S_APP_ROM
    .model : ALIGN(4)
    {
        *person_detect_model_data_vela.o*(.text* .rodata*)
    } > CM55M_S_SRAM
    .noinit (NOLOAD): ALIGN(4)
    {
        *(.bss.NoInit*)
        *(.noinit*)
    } > CM55M_S_SRAM
    .rodata : ALIGN(4)
    {
        __rodata_start = .;
        *(.rodata .rodata.* .constdata .constdata.*)
        __rodata_end = .;
    } > CM55M_S_APP_DATA
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > CM55M_S_APP_ROM
    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > CM55M_S_APP_ROM
   .copy.table :
   {
     . = ALIGN(4);
     __copy_table_start__ = .;
        LONG(LOADADDR(.data));
        LONG( ADDR(.data));
        LONG( SIZEOF(.data)/4);
     __copy_table_end__ = .;
   } > CM55M_S_APP_ROM
   .zero.table :
   {
     . = ALIGN(4);
     __zero_table_start__ = .;
        LONG( ADDR(.bss));
        LONG( SIZEOF(.bss)/4);
     __zero_table_end__ = .;
   } > CM55M_S_APP_ROM
     . = ALIGN(32);
    __unprivileged_flash_end__ = (. - 1);
    .data : ALIGN(4)
    {
       FILL(0xff)
    __data_start__ = .;
       . = ALIGN(32);
       __privileged_sram_start__ = .;
       *(privileged_data)
       *(privileged_data*)
       . = ALIGN(32);
       __privileged_sram_end__ = (. - 1);
        *(vtable)
       *(.data)
       *(.data.*)
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
       . = ALIGN(4) ;
     __data_end__ = .;
    } > CM55M_S_APP_DATA
  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > CM55M_S_APP_DATA
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        PROVIDE(__start_noinit_RAM = .) ;
        PROVIDE(__start_noinit_SRAM = .) ;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
       PROVIDE(__end_noinit_RAM = .) ;
       PROVIDE(__end_noinit_SRAM = .) ;
    } > CM55M_S_APP_DATA
   .heap (COPY) :
   {
     . = ALIGN(8);
     __HeapBase = .;
     PROVIDE(__HeapBase = .);
     end = __HeapBase;
     . = . + __HEAP_SIZE;
     . = ALIGN(8);
     __HeapLimit = .;
     PROVIDE(__HeapLimit = .);
   } > CM55M_S_APP_DATA
   .stack (ORIGIN(CM55M_S_APP_DATA) + LENGTH(CM55M_S_APP_DATA) - __STACK_SIZE) (COPY) :
   {
     . = ALIGN(8);
     __StackLimit = .;
     PROVIDE(__StackLimit = .);
     . = . + __STACK_SIZE;
     . = ALIGN(8);
     __StackTop = .;
     PROVIDE(__StackTop = .);
   } > CM55M_S_APP_DATA
   PROVIDE(__stack = __StackTop);
   ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}
