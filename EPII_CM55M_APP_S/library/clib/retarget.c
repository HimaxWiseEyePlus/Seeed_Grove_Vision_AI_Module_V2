#if defined (ARMCM55)
#include "WE2_device.h"
#else
#error device not specified!
#endif

#if !defined(HX_TFM)
#include "console_io.h"

int stdin_getchar(void) {
    return console_getchar();
}

int stdout_putchar(int ch) {
    console_putchar(ch);
    return (ch);
}

int stderr_putchar(int ch) {
    console_putchar(ch);
    return (ch);
}

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
//ARM CLANG COMPILER 
void ttywrch(int ch) {
    stdout_putchar(ch);
}
#else //GCC COMPILER 
#ifdef JACKY
void __malloc_lock() {
}
void __malloc_unlock() {
}
#endif
#endif

#endif //#if !defined(HX_TFM) 

#if defined(HX_TFM)
#include "uart_stdout.h"

int stdin_getchar(void) {
    unsigned char ch;
    stdio_input_string(&ch, 1);
    return (ch);
}

int stdout_putchar(int ch) {
    stdio_output_string((const unsigned char *)&ch, 1);
    return (ch);
}

int stderr_putchar(int ch) {
    stdio_output_string((const unsigned char *)&ch, 1);
    return (ch);
}

#endif //#if defined(HX_TFM)
