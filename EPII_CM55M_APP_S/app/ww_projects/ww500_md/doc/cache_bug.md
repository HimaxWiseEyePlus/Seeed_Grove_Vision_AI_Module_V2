# Incorrect use of Cache Management Functions when Writing Image to Disk
#### CGP - 15/7/25

We have previously been puzzled why we have had trouble inserting EXIF data into JPEG files
before writing them to the SD card. We had not found the problem and coded around it.

In a new build we found that the jpeg file on the SD card contained data that bore
no likeness to the data buffer contents. Investgation has shown this seems to be due to 
a bug in the cache code in the `fastfs_write_image()` function, 
which is responsible for writing the file.

__Summary: The file write code uses `SCB_InvalidateDCache_by_Addr()` instead of
`SCB_CleanDCache_by_Addr()`. Intended data is lost and junk is written.__

The code (shortened for clarity) is:
```
int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename) {

    res = f_open(&fil_w, filename, FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK)
    {
        SCB_InvalidateDCache_by_Addr ((void *)SRAM_addr, img_size);
        res = f_write(&fil_w, (void *)SRAM_addr, img_size, &bw);
        f_close(&fil_w);
    }
}
```
Using `SCB_CleanDCache_by_Addr()` gives the intended result.

Not that this does not happen always: we have written many files with the original code without
a problem. We suspect that some other change in our code, perhaps affecting the use of buffers
and perhaps chance, has caused the problem to manifest itself.

 ## Diagnosis
 
 I added code at various places that printed the first few bytes of what was to become
 the JPEG file. These have a recognisable pattern and it was clear that the buffer
 had the correct contents until after the `SCB_InvalidateDCache_by_Addr()` call,
 at which point the file was written with the wrong data. 
 
 I noticed the `SCB_InvalidateDCache_by_Addr()` call before and not understood it, 
 and although I suspected that earlier problems might be cache-related I had not investigated this before.
 
I had a very helpful [conversation with ChatGPT](https://chatgpt.com/share/687583ae-c920-8005-ba60-0c2e75fe797b).
This explained what `SCB_InvalidateDCache_by_Addr()` does. When I explained the symptoms I got this:
```
Yes, absolutely — using SCB_InvalidateDCache_by_Addr() before f_write() is very likely the cause of your 
corrupted or incorrect data on the SD card.

SCB_InvalidateDCache_by_Addr() discards any data in the data cache for the specified memory range. 
If the CPU modified the buffer (e.g. EXIF inserted, JPEG data arranged), and the buffer resides in 
cached memory (e.g. normal RAM), those writes may be only in the D-cache, not yet flushed to actual RAM.

When you then invalidate the cache before calling f_write(), you effectively discard those changes, 
and f_write() reads stale data from RAM — possibly empty, partially old, or garbage.
```

## Double-Checking

I introduced a flag `CACHEFIX` and used it in `fastfs_write_image()` - relevant parts shown below.

```
#ifdef CACHEFIX
	// This ensures that any data in the D-cache is committed to RAM
	SCB_CleanDCache_by_Addr ((void *)SRAM_addr, img_size);
	xprintf("Used 'SCB_CleanDCache' - writing %d bytes beginning:\n", img_size);    
	// I printed some of SRAM_addr[] here
#else
	// This discards any data in the data cache for the specified memory range
	SCB_InvalidateDCache_by_Addr ((void *)SRAM_addr, img_size);
	xprintf("Used 'SCB_InvalidateDCache' - writing %d bytes beginning:\n", img_size);
	// I printed some of SRAM_addr[] here
#endif // CACHEFIX
	res = f_write(&fil_w, (void *)SRAM_addr, img_size, &bw);
```
Sure enough, without `CACHEFIX` the `SRAM_addr[]` buffer contains junk, and with `CACHEFIX` it is OK. The console output
in each case follows. The initial lines are written by the calling code and includes the expected data which begins `FF D8 FF E1`.

For the `SCB_InvalidateDCache_by_Addr()` case, the buffer contents is different:
```
JPEG&EXIF buffer (8976 bytes) begins:
FF D8 FF E1 01 52 45 78 69 66 00 00 49 49 2A 00

Used 'SCB_InvalidateDCache' - writing 8976 bytes beginning:
75 7C 1A 66 78 E6 DD 97 68 66 FC 09 4C 08 17 C4
```

For the `SCB_CleanDCache_by_Addr()` case, the buffer contents is identical:
```
JPEG&EXIF buffer (8820 bytes) begins:
FF D8 FF E1 01 52 45 78 69 66 00 00 49 49 2A 00

Used 'SCB_CleanDCache' - writing 8820 bytes beginning:
FF D8 FF E1 01 52 45 78 69 66 00 00 49 49 2A 00
```

## Use of Cache Management Instructions Elsewhere

The Himax code defines these, and uses at least some of them:

* SCB_InvalidateDCache_by_Addr()
* SCB_CleanDCache_by_Addr()
* hx_InvalidateDCache_by_Addr()	// A wrapper around SCB_InvalidateDCache_by_Addr()
* hx_CleanDCache_by_Addr()		// A wrapper around SCB_CleanDCache_by_Addr()

I have not checked to see whether these are used correctly.



