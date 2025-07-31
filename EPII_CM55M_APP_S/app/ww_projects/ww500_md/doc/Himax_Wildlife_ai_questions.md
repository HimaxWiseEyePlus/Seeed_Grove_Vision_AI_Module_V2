# Questions and Issues from Wildlife.ai to Himax
#### Charles Palmer 29 July 2025

A number of issues and questions have arisen. Some of these we have resolved or worked around.


#### 1.  Compiler Selection and Library Choice

Summary: The makefiles are linking the full C libc library and not the small nano library.
Changes to the latest GCC toolchain were an aggravating factor that resulted in a size blow-out. By specifying nano library
then code size shrinks and is similar with either compiler.

The issue has been posted [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/issues/86).

#### 2. Incorrect use of Cache Management Functions when Writing Image to Disk

Summary: The file write code uses SCB_InvalidateDCache_by_Addr() instead of
SCB_CleanDCache_by_Addr(). Intended data is lost and junk is written.

The issue has been posted [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/issues/85).


#### 3.  Source code of live streaming of video

#### 4. HM0360 Sleep current



#### EXIF insertion

#### Can we add XIP SRAM?

#### HM0360 MD with RP cameras

#### LED flash circuit for both cameras

#### Restructure SDK

