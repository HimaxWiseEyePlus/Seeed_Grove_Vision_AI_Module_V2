#include "WE2_device_addr.h"
MEMORY
{
  /* Define each memory region */
  CM55M_S_APP_ROM (rx) : ORIGIN = 0x10000000, LENGTH = 0x40000 /* 256K bytes  */  
  CM55M_S_APP_DATA (rwx) : ORIGIN = 0x30000000, LENGTH = 0x40000 /* 256K bytes*/ 
  CM55M_S_SRAM (rwx) : ORIGIN = BOOT2NDLOADER_BASE, LENGTH = 0x00200000-(BOOT2NDLOADER_BASE-BASE_ADDR_SRAM0_ALIAS) /* 2M-0x1f000 bytes*/
}

/* .map file says:
Memory Configuration

Name             Origin             Length             Attributes
CM55M_S_APP_ROM  0x10000000         0x00040000         xr
CM55M_S_APP_DATA 0x30000000         0x00040000         xrw
CM55M_S_SRAM     0x3401f000         0x001e1000         xrw
*default*        0x00000000         0xffffffff

 */

/* CGP 11/7/25 When we switched from compiler 10.3 to 14.3 it triggered this error from below:
  	// Check if data + heap + stack exceeds RAM limit 
  	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

Apparently the memory is used as follows:

| origin = 0x30000000 ------------------→ (grows upward)
| [.data]
| [.bss]
| [.heap]     (__HeapBase → __HeapLimit)
| ...         overlap triggers error here
| [.stack]    (__StackLimit → __StackTop)
| end = 0x30040000


*/

/* CGP 10/7/25 - I reduced heap and stack sizes and it resolves the linker error.
But what is optimum?

Was:
__HEAP_SIZE = 0x10000;
__STACK_SIZE = 0x10000;
 Now:
*/
__HEAP_SIZE = 0xa000;
__STACK_SIZE = 0xa000;

ENTRY(Reset_Handler)

SECTIONS
{
    /* MAIN TEXT SECTION */
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
    } > CM55M_S_SRAM

    .model : ALIGN(4)
    {
        *person_detect_model_data_vela.o*(.text* .rodata*)
    } > CM55M_S_SRAM
  
    .noinit (NOLOAD): ALIGN(4)
    {
        *(.bss.NoInit*)
        *(.noinit*)
    } > CM55M_S_SRAM

    .text : ALIGN(4)
    {
       *(.text*)
       KEEP(*freertos*/tasks.o(.rodata*)) /* FreeRTOS Debug Config */
       . = ALIGN(4);
       KEEP(*(.init))

       KEEP(*(.fini));
            
    	/* .ctors */
    	*crtbegin.o(.ctors)
    	*crtbegin?.o(.ctors)
    	*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    	*(SORT(.ctors.*))
    	*(.ctors)

    	/* .dtors */
    	*crtbegin.o(.dtors)
    	*crtbegin?.o(.dtors)
    	*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    	*(SORT(.dtors.*))
    	*(.dtors)
        . = ALIGN(4);
        
        KEEP(*(.eh_frame*))
    } > CM55M_S_APP_ROM

    .rodata : ALIGN(4)
    {
        __rodata_start = .;
        *(.rodata .rodata.* .constdata .constdata.*)
        __rodata_end = .;
    } > CM55M_S_APP_DATA

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
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
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data)/4);
	
	    /* Add each additional data section here */
	    __copy_table_end__ = .;
	  } > CM55M_S_APP_ROM
              
	  .zero.table :
	  {
	    . = ALIGN(4);
	    __zero_table_start__ = .;
	    /* Add each additional bss section here */
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss)/4);    
	    __zero_table_end__ = .;
	  } > CM55M_S_APP_ROM
                
     . = ALIGN(32);
    __unprivileged_flash_end__ = (. - 1);
  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section in RAM
   * which must be 4byte aligned
   */      
    /* Main DATA section (BOOTROM_SRAM) */
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
       /* preinit data */
       PROVIDE_HIDDEN (__preinit_array_start = .);
       KEEP(*(.preinit_array))
       PROVIDE_HIDDEN (__preinit_array_end = .);

       . = ALIGN(4);
       /* init data */
       PROVIDE_HIDDEN (__init_array_start = .);
       KEEP(*(SORT(.init_array.*)))
       KEEP(*(.init_array))
       PROVIDE_HIDDEN (__init_array_end = .);


       . = ALIGN(4);
       /* finit data */
       PROVIDE_HIDDEN (__fini_array_start = .);
       KEEP(*(SORT(.fini_array.*)))
       KEEP(*(.fini_array))
       PROVIDE_HIDDEN (__fini_array_end = .);

       KEEP(*(.jcr*))
       . = ALIGN(4) ;
    	/* All data end */
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

    /* DEFAULT NOINIT SECTION */
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

    /* Reserve and place Heap within memory map */
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
  
    /* Locate actual Stack in memory map */
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

  	/* Check if data + heap + stack exceeds RAM limit */
  	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
  
    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;

	/* Calculate RAM usage and margins as symbols */
	/* Suggested by Copilot. Can be printed by printLinkerStats() */
	PROVIDE(__RAM_Start      = ORIGIN(CM55M_S_APP_DATA));
	PROVIDE(__RAM_End        = ORIGIN(CM55M_S_APP_DATA) + LENGTH(CM55M_S_APP_DATA));
	PROVIDE(__RAM_Size       = LENGTH(CM55M_S_APP_DATA));

	/* .rodata diagnostics */
	PROVIDE(__Rodata_Start = ADDR(.rodata));
	PROVIDE(__Rodata_End   = ADDR(.rodata) + SIZEOF(.rodata));
	PROVIDE(__Rodata_Size   = SIZEOF(.rodata));
	/* .copy.table diagnostics */
	PROVIDE(__CopyTable_Start = ADDR(.copy.table));
	PROVIDE(__CopyTable_End   = ADDR(.copy.table) + SIZEOF(.copy.table));
	PROVIDE(__CopyTable_Size  = SIZEOF(.copy.table));
	/* .zero.table diagnostics */
	PROVIDE(__ZeroTable_Start = ADDR(.zero.table));
	PROVIDE(__ZeroTable_End   = ADDR(.zero.table) + SIZEOF(.zero.table));
	PROVIDE(__ZeroTable_Size  = SIZEOF(.zero.table));
	/* Exception table diagnostics */
	PROVIDE(__ARM_Extab_Size = SIZEOF(.ARM.extab));
	PROVIDE(__ARM_Exidx_Size = SIZEOF(.ARM.exidx));
	/* .privileged_data diagnostics */
	PROVIDE(__PrivilegedData_Start = ADDR(.data));
	PROVIDE(__PrivilegedData_End   = __PrivilegedData_Start + (__privileged_sram_end__ - __privileged_sram_start__));
	PROVIDE(__PrivilegedData_Size  = (__privileged_sram_end__ - __privileged_sram_start__));
	PROVIDE(__PrivilegedSRAM_Start = __privileged_sram_start__);
	PROVIDE(__PrivilegedSRAM_End = __privileged_sram_end__);

	PROVIDE(__noinit_SRAM_Size  = (__end_noinit_SRAM - __start_noinit_SRAM));

	PROVIDE(__Data_Start     = ADDR(.data));
	PROVIDE(__Data_End       = ADDR(.data) + SIZEOF(.data));
	PROVIDE(__Data_Size      = SIZEOF(.data));
	PROVIDE(__BSS_Start      = ADDR(.bss));
	PROVIDE(__BSS_End        = ADDR(.bss) + SIZEOF(.bss));
	PROVIDE(__BSS_Size        = SIZEOF(.bss));

	PROVIDE(__HeapSize       = __HeapLimit - __HeapBase);
	PROVIDE(__StackSize      = __StackTop - __StackLimit);
	PROVIDE(__HeapStackMargin = __StackLimit - __HeapLimit); /* Key margin value */

}
