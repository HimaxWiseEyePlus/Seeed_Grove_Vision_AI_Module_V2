# Compiler Selection and Library Choice
#### CGP 13/7/25

__One-line summary: The problem is that the code was linking the full C libc library and not the small nano library.
Changes to the latest compiler were an aggravating factor that resulted in a size blow-out. By specifying nano library
then code size shrinks and is similar with either compiler.__

Tobyn encountered a problem building the ww500_md project which I had uploaded to Github. The problem manifest 
with this message: `region RAM overflowed with stack`

This error comes from the linker file which has this test:
```
  	/* Check if data + heap + stack exceeds RAM limit */
  	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
```

After quite a bit of work it turned out that the problem happened with his compiler which is arm-none-eabi-gcc 14.3 Rel1
and I had been using arm-none-eabi-gcc 10.3.1

The two versions produce different outputs and in his case the stack encroached on the heap, giving the error. 
Apparently the memory is used as follows (more or less):

```
| origin = 0x30000000 ------------------→ (grows upward)
| [.data]
| [.bss]
| [.heap]     (__HeapBase → __HeapLimit)
| ...         overlap triggers error here
| [.stack]    (__StackLimit → __StackTop)
| end = 0x30040000
```

The short-term fix was to change the stack and heap allocations in the linker file from this:
```
__HEAP_SIZE = 0x10000;
__STACK_SIZE = 0x10000;
```
to this:
```
__HEAP_SIZE = 0x8000;
__STACK_SIZE = 0x8000;
```
and then later to this:
```
__HEAP_SIZE = 0xa000;
__STACK_SIZE = 0xa000;
```
Then the code compiled and ran, but the question is what caused the probelm when the compiler version changed.

## My Experiments with Compiler Versions

It turned out that my Eclipse was picking up the GCC tools from the PATH on my Windows laptop.
The version of code I was using was 10.3.? and had been installed when I was using ST Micro tools.

I downloaded the "latest" which is 14.3 Rel 1 from [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
It turned out that if I used 14.3 Rel 1 I got the same error as Tobyn.
 
Later I also downloaded the official 10.3.1 from [here](https://developer.arm.com/downloads/-/gnu-rm),
so I could rule out something funny with the ST version 10.3.
So I could switch between versions, in my Eclipse Properties > C/C++ Build > Environment section I added 
(ultimately) 4 environment variables shown in the table below. I copied the value of one of the three lower entried
to `GNU_TOOLPATH` to select the version to compile with:

| Name        | Value (location on my laptop of the tools)  | Notes |
|-------------|---------|-------|
| GNU_TOOLPATH      | (One of the other values copied from below) | This is the toolchain that Eclipse will use. |
| GNU_TOOLPATH_14.3 | C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\10 2021.10\bin | Official toolchain, v10.3.1 |
| GNU_TOOLPATH_10.3 | C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin | Official toolchain, v14.3 Rel 1|
| GNU_TOOLPATH_ST   | C:/ST/STM32CubeIDE_1.10.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.0.202111181127/tools/bin | This is the ST Micro version I had been using. |

Interestingly I have just noted that the bootloader prints this (which tells us what Himax was using):
```
Compiler Version: ARM GNU, 10.3.1 20210824 (release)
```

## Diagnosis - Printing Linker Sizes

As part of the process of understanding the fault I wanted more information on why we get the 
`region RAM overflowed with stack` message.
 
It turns out that values in use by the linker can be designated with a `PROVIDE` command 
so that they can be accessed by C code at run time. As an example, this:
```
	/* .rodata diagnostics */
	PROVIDE(__Rodata_Start = ADDR(.rodata));
	PROVIDE(__Rodata_End   = ADDR(.rodata) + SIZEOF(.rodata));
	PROVIDE(__Rodata_Size   = SIZEOF(.rodata));
```
can be used by C code like this:
```
extern uint32_t __Rodata_Start;
extern uint32_t __Rodata_End;
extern uint32_t __Rodata_Size;

static void printLinkerStats(void) {
	/* .rodata diagnostics */
    xprintf(".rodata 		0x%08x-0x%08x (%lu bytes)\n",
    	    (unsigned long)&__Rodata_Start, (unsigned long)&__Rodata_End, (unsigned long)&__Rodata_Size);
}
```
which gives this console output:
```
.rodata         0x30000000-0x30007396 (29590 bytes)
```	

I added quite a lot of these PROVIDE links and corresponding xprintf() lines. See the [ww500_md.ld](../ww500_md.ld)
and the `PRINTLINKERSTATS` sections in [ww500_md.c](../ww500_md.c). 

Sample console output follows, 
for different compilers (and - later - libraries). At the top I have added the version output string 
and linker size output lines. In all cases the stack and heap allocation
is 0xa000. As you can see the numbers for the first 3 cases are similar except for `.rodata` which is much larger for 
14.3, and which affects Heap Base and so the Heap/Stack Margin.

Interestingly, although the version strings for the two 10.3 toolchains are very similar,
the sizes are slightly different.

#### ST Micro Version 10.3
```
arm-none-eabi-gcc (GNU Tools for STM32 10.3-2021.10.20211105-1100) 10.3.1 20210824 (release)

Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      140368 B       256 KB     53.55%
CM55M_S_APP_DATA:      110792 B       256 KB     42.26%
    CM55M_S_SRAM:      983296 B      1924 KB     49.91%    
    
***** Linker Stats *****
RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x30007396 (29590 bytes)
.copy.table     0x1002243c-0x10022448 (12 bytes)
.zero.table     0x10022448-0x10022450 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x30007398-0x30007397 (4294967295 bytes)
Privileged SRAM 0x300073a0-0x3000739f
Data            0x30007398-0x30008d10 (6520 bytes)
BSS             0x30008d10-0x3001b0c8 (74680 bytes)
Heap Base:      0x3001b0c8
Heap Limit:     0x300250c8
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 69432 bytes
```

#### Official version 10.3.1
```
arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)

Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      150624 B       256 KB     57.46%
CM55M_S_APP_DATA:      111208 B       256 KB     42.42%
    CM55M_S_SRAM:      983296 B      1924 KB     49.91%

RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x30007530 (30000 bytes)
.copy.table     0x10024c4c-0x10024c58 (12 bytes)
.zero.table     0x10024c58-0x10024c60 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x30007530-0x3000752f (4294967295 bytes)
Privileged SRAM 0x30007540-0x3000753f
Data            0x30007530-0x30008eb0 (6528 bytes)
BSS             0x30008eb0-0x3001b268 (74680 bytes)
Heap Base:      0x3001b268
Heap Limit:     0x30025268
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 69016 bytes
```

#### Official 14.3 Rel 1
```
arm-none-eabi-gcc (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 14.3.1 20250623

Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      154872 B       256 KB     59.08%
CM55M_S_APP_DATA:      137688 B       256 KB     52.52%
    CM55M_S_SRAM:      983072 B      1924 KB     49.90%
    
***** Linker Stats *****
RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x3000df38 (57144 bytes)
.copy.table     0x10025ce4-0x10025cf0 (12 bytes)
.zero.table     0x10025cf0-0x10025cf8 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x3000df38-0x3000df37 (4294967295 bytes)
Privileged SRAM 0x3000df40-0x3000df3f
Data            0x3000df38-0x3000f780 (6216 bytes)
BSS             0x3000f780-0x300219d8 (74328 bytes)
Heap Base:      0x300219d8
Heap Limit:     0x3002b9d8
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 42536 bytes
```
## 14.3 with nano library
I re-built the app using the nano library - see below for a discussion.
```
arm-none-eabi-gcc (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 14.3.1 20250623

Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      107424 B       256 KB     40.98%
CM55M_S_APP_DATA:      106736 B       256 KB     40.72%
    CM55M_S_SRAM:      983072 B      1924 KB     49.90%
    
***** Linker Stats *****
RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x30007059 (28761 bytes)
.copy.table     0x1001a38c-0x1001a398 (12 bytes)
.zero.table     0x1001a398-0x1001a3a0 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x30007060-0x3000705f (4294967295 bytes)
Privileged SRAM 0x30007060-0x3000705f
Data            0x30007060-0x30008074 (4116 bytes)
BSS             0x30008078-0x3001a0f0 (73848 bytes)
Heap Base:      0x3001a0f0
Heap Limit:     0x300240f0
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 73488 bytes    
```    

## 10.3 with nano library
I re-built the app using the nano library - see below for a discussion.

```
Memory region         Used Size  Region Size  %age Used
 CM55M_S_APP_ROM:      111312 B       256 KB     42.46%
CM55M_S_APP_DATA:      107832 B       256 KB     41.13%
    CM55M_S_SRAM:      983296 B      1924 KB     49.91%
    
***** Linker Stats *****
RAM             0x30000000-0x30040000 (262144 bytes)
.rodata         0x30000000-0x30007192 (29074 bytes)
.copy.table     0x1001b2bc-0x1001b2c8 (12 bytes)
.zero.table     0x1001b2c8-0x1001b2d0 (8 bytes)
.ARM.extab_Size: 0x00000000
.ARM.exidx_Size: 0x00000008
PrivilegedData  0x30007198-0x30007197 (4294967295 bytes)
Privileged SRAM 0x300071a0-0x3000719f
Data            0x30007198-0x300081bc (4132 bytes)
BSS             0x300081c0-0x3001a538 (74616 bytes)
Heap Base:      0x3001a538
Heap Limit:     0x30024538
Stack Top:      0x30040000
Stack Limit:    0x30036000
Heap size:      40960 bytes
Stack size:     40960 bytes
Heap/Stack margin: 72392 bytes    
```
    
## Linker `no-warn-rwx-segments` Option

There is a small, annoying issue in `toolchain_gnu.mk:
```
	ifeq ($(firstword $(sort $(GCC_VERSION) 12.0.0)),12.0.0)
	# if GCC >= 12.0.0, use -Wl,--no-warn-rwx-segments
	LINK_OPT	+= -Wl,--no-warn-rwx-segments
	endif
```    
It looks like this is an option that is recognised by 143. but not but 10.3. The `ifeq`
line is supposed to omit the option with the 10.3 compiler but it does not work. Unless it is
commented out then the 10.3 linker gives this error:
```
-eabi/10.3.1/../../../../arm-none-eabi/bin/ld.exe: unrecognized option '--no-warn-rwx-segments'
```
However if it is commented out the the 14.3 linker gives this warning:
```
EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf has a LOAD segment with RWX permissions
```
I have not managed to trace the root cause of the problem - which looks like it is a failure
to create the `GCC_VERSION` value. The following line does not print the version:
```
$(info Detected GCC version: $(GCC_VERSION))
```

## What is .rodata?

From the above diagnostics it appears that the increase on memory use is in the `.rodata` section. I decided I needed
to dig into this. 

The HX6538 is unusual in that the flash is external QSPI serial EEPROM, whose contents is copied to SRAM at boot.
Furthermore, there are two chunks of SRAM, as indiacted in these lines from the linker file:
```
  CM55M_S_APP_DATA (rwx) : ORIGIN = 0x30000000, LENGTH = 0x40000 /* 256K bytes*/ 
  CM55M_S_SRAM (rwx) : ORIGIN = BOOT2NDLOADER_BASE, LENGTH = 0x00200000-(BOOT2NDLOADER_BASE-BASE_ADDR_SRAM0_ALIAS) /* 2M-0x1f000 bytes*/
```
So although .rodata is normally thought of as "read only data" and placed in flash, in our case it is placed in
(both of) the two sections above, which are both SRAM.

I asked ChartGPT: "What is .rodata used for, and why might it change between GNU C compiler versions 10.3 and 14.3?"
 and I got this reply:
 
Great question — understanding what `.rodata` is and why it changes between GCC versions like 10.3 and 14.3 is key to debugging size regressions.

---

## 📦 What is `.rodata`?

`.rodata` is a **read-only data segment** in your binary that holds:

| Kind of Data      | Examples                           |
| ----------------- | ---------------------------------- |
| Constant literals | Strings like `"Hello, world"`      |
| Lookup tables     | `const int table[] = { ... };`     |
| C++ vtables       | `virtual` function dispatch tables |
| Typeinfo & RTTI   | `typeid(...)`, `dynamic_cast`      |
| Static const data | `static const char banner[] = ...` |

It’s memory-mapped as **read-only**, often to save RAM or enforce immutability.

---

## 🔍 Why Might `.rodata` Grow Between GCC 10.3 and 14.3?

### ✅ 1. **Improved (or more conservative) inlining**

* GCC 14 may **inline more aggressively**, duplicating `const` data across translation units
* This results in **multiple copies of the same lookup tables or strings**

### ✅ 2. **Reduced literal deduplication**

* Older GCC might deduplicate string literals across translation units
* GCC 14 may only deduplicate within a TU unless `-fmerge-all-constants` is used

### ✅ 3. **Standard library growth**

* Newer `libstdc++` may include larger `std::string`, `std::bad_alloc`, `typeinfo` symbols
* C++ runtime data (vtable, RTTI, exception tables) is often stored in `.rodata`

### ✅ 4. **New defaults in GCC 14**

* Some `-f*` flags changed defaults (e.g., `-fno-merge-constants`)
* Certain debug or LTO-related flags might expand `.rodata`

### ✅ 5. **Longer mangled symbol names**

* GCC 14 may introduce more detailed symbol names or metadata, especially in templated or exception code

### ✅ 6. **More elaborate internal lookup tables**

* Some math, printf, or format-related internals use `const` data tables that now end up in `.rodata`

---

## 🔬 Common Culprits in Practice

* **String constants**: repeated across `.cpp` files
* **Static const arrays**: duplicated due to inlining
* **`typeid`, `dynamic_cast`, virtuals**: all use RTTI in `.rodata`
* **Templates**: expanded with slightly different instantiations
* **Third-party libraries**: e.g. Newlib or libc can include large error message strings

---

## 🛠 What You Can Do

### 🔧 Compiler Options:

Add these flags:

```bash
-Os -fmerge-all-constants -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions
```

### 🔍 Investigate `.rodata`:

* Use your map comparison script to **pinpoint symbols that grew**
* Look for duplicated tables or strings
* Look for standard library bloat from `<iostream>`, exceptions, etc.

### 🧹 Cleanups:

* **Avoid virtuals** and RTTI in embedded
* Share string constants via `extern const char*`
* Move large lookup tables to RAM (if needed)

---

## 🧠 Summary

| Cause                 | What Changed?                            |
| --------------------- | ---------------------------------------- |
| Literal deduplication | Might be less aggressive in GCC 14       |
| Inlining behavior     | Might duplicate constants                |
| C++ standard libs     | Larger RTTI/typeinfo tables              |
| Compilation flags     | `-fmerge-constants` may be off           |
| Embedded libc         | Different defaults (e.g. Newlib vs Nano) |

Your script is already the perfect tool to drill into these changes!

Let me know if you'd like help spotting common `.rodata` bloat types in your specific output.

## Diagnosis - Processing .map Files

At one stage I thought it would be useful to find out what was contributing to the increas in .rodata sizes.
I started a process, first with Copilot and then with ChatGPT, to get a Python script that
would print functions that contributed to .rodata, ordered by size, and comparing the 
outputs of the two compilers 10.3 and 14.3. It turned out I also needed to distinguish between
the two different SRAM areas which contained .rodata.

This took too long and many tries. In the end ChatGPT produced [compare_rodata_maps.py](../../../../../_Tools/compare_rodata_maps.py) 
The first few lines of output are shown below:

```
# Map file 1: c103map.txt
# Map file 2: c143map.txt

# Region CM55M_S_APP_DATA: 33949 bytes (map1), 86561 bytes (map2), delta = 52612 bytes
# Region CM55M_S_APP_ROM: 25 bytes (map1), 47 bytes (map2), delta = 22 bytes
# Region CM55M_S_SRAM: 256758 bytes (map1), 259548 bytes (map2), delta = 2790 bytes
# Region Unknown: 11238 bytes (map1), 15277 bytes (map2), delta = 4039 bytes

Region             |    Size1 |    Size2 |   Delta | Symbol                | Object File
--------------------------------------------------------------------------------------------------
CM55M_S_APP_DATA   |       67 |    26204 |   26137 | _exit.str1.4          | gnu/retarget_io.o
CM55M_S_APP_DATA   |        0 |    13788 |   13788 | categories            | <stdlib>
CM55M_S_APP_DATA   |        0 |     6780 |    6780 | d02f4                 | <stdlib>
CM55M_S_APP_DATA   |        0 |     5930 |    5930 | b02cf                 | <stdlib>
CM55M_S_APP_DATA   |     2281 |       76 |   -2205 | str1.4                | <stdlib>
```
I did not finalise this script but the work gave me enough clues to recognise the problem was with the 
libc libraries.

## Libraries

Late in the investigation it became clear that the image was being linked with large standard libraries:

I asked ChatGPT about libraries, copying it a line from one of the .map files:
```
 .bss           0x0000000000000000        0x0 c:/st/stm32cubeide_1.10.1/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.0.202111181127/tools/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/lib/thumb/v8-m.main+dp/hard\libc.a(lib_a-assert.o)
```
The reply:

This confirms:

* You are using newlib as your C library.
* The file libc.a is the static archive of newlib (not glibc or musl).
* lib_a-assert.o is a compiled object file from the newlib source (assert.c).
* Path indicates the use of Thumb-2 instructions targeting ARMv8-M with Mainline profile and hardware floating-point (hard ABI).

So:

✅ You're using newlib, and more specifically, the full newlib (not newlib-nano) — unless you've configured otherwise.

This is apparently a linker flag option:
```
--specs=nano.specs
```
and although lines to that effect exist in `toolchain_gnu.mk` they are not being invoked. So I added this and recompiled:
```
	LINK_OPT += -specs=nano.specs
	LINK_OPT += -specs=nosys.specs
```
I did this with both 14.3 and 10.3 compilers and the results show:
* Much lower memory use.
* Very little difference between the two compilers.

__Problem Solved.__

But the work does point to quite a few issues with theh makefiles, which are quite complex.


