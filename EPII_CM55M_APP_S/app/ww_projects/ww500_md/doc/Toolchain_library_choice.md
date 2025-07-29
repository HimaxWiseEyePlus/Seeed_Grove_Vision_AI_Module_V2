# Makefile selects newlib not newlib-nano

__One-line summary: The makefiles are linking the full C libc library and not the small nano library.
Changes to the latest GCC toolchain were an aggravating factor that resulted in a size blow-out. By specifying nano library
then code size shrinks and is similar with either compiler.__

The encountered a memory blow-out problem when a colleague built our code. The message was "region RAM overflowed with stack".

On investigation we found that the colleague was using a later GCC toolchain (14.3 Rel 1) while I had been using 10.3.

The question was why the compiler difference gave a difference in code size. We found that the link process was using the full-size 
newlib library, rather than the newlib-nano library. We assume that with the new compiler the newlib library became
significantly larger which gave the memory allocation error.

This table shows the differences between toolchains and libraries, on our project:

| Toolchain Version | Library | .rodata size |   |
| ----------------- | --------|--------------|---|
| 10.3.1            | libc    | 30000 bytes  |   |
| 10.3.1            | nano    | 29074 bytes  |   |
| 14.3 Rel 1        | libc    | 57144 bytes  | Blowout! |
| 14.3 Rel 1        | nano    | 28761 bytes  |   |

The fix was to add `LINK_OPT += -specs=nano.specs` into `toolchain_gnu.mk` at line 236. 
(We also added `LINK_OPT += -specs=nosys.specs` at the same place though that may not have been significant).
That not only fixed the memory blow-out problem, but the memory use was significantly smaller with either toolchain.

Note there are other occurrances of `-specs=nano.specs` in that makefile but they were evidently not being invoked.
I did not attempt to understand why.

#### Linker `no-warn-rwx-segments` Option

There is another small, annoying issue in `toolchain_gnu.mk` - that may possibly be related.
```
	ifeq ($(firstword $(sort $(GCC_VERSION) 12.0.0)),12.0.0)
	# if GCC >= 12.0.0, use -Wl,--no-warn-rwx-segments
	LINK_OPT	+= -Wl,--no-warn-rwx-segments
	endif
```    
It looks like `no-warn-rwx-segments` is an option that is recognised by 14.3 but not but 10.3. The `ifeq`
line is supposed to omit the option with the 10.3 compiler but it does not work. Unless it is
commented out (as shown above) then the 10.3 linker gives this error:
```
-eabi/10.3.1/../../../../arm-none-eabi/bin/ld.exe: unrecognized option '--no-warn-rwx-segments'
```
However if it is commented out then the 14.3 linker gives this warning:
```
EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf has a LOAD segment with RWX permissions
```
I have not managed to trace the root cause of the problem - which looks like it is a failure
to create the `GCC_VERSION` value. The following line does not print the version:
```
$(info Detected GCC version: $(GCC_VERSION))
```
