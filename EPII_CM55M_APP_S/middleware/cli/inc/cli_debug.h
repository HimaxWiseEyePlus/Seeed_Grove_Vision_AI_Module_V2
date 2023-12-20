/******************************************************************************
 * MODULE NAME: com_debug.h
 * PURPOSE:  Debug, Error and Warning message routine function header file
 *
 * NOTES:
 *
 * CREATOR: roc Tsay (11/30/2004)
 * Revision Log:
 * Copyright(C) Himax Technology, 2004
 *******************************************************************************/

#ifndef _DEBUG_MESSAGE_INC_HEADER
#define _DEBUG_MESSAGE_INC_HEADER

// ++++++++++++++++++++++++++++
// NAMING CONSTANT DECLARATIONS
// ++++++++++++++++++++++++++++
#include <stdio.h>

// status code definition 0X1---- ~ 0XFF---- ( - means can not use )
// Success code
#define STATUS_RETURN_SUCCESS                   0x00000000
// Error Code
#define STATUS_RETURN_FAIL                      0xFFFFFFFF
#define STATUS_RETURN_OPEN_ERROR                0x80000001
#define STATUS_RETURN_CLOSE_ERROR               0x80000002
#define STATUS_RETURN_IOCTL_ERROR               0x80000003
#define STATUS_RETURN_DRV_NON_INIT              0x80000004
#define STATUS_RETURN_PARAM_ERROR               0x80000005
#define STATUS_RETURN_NOT_FINISH                0x80000006
#define STATUS_RETURN_ALLOC_MEM_ERROR           0x80000007
#define STATUS_RETURN_MMAP_ERROR                0x80000008
#define STATUS_RETURN_NO_RESOURCE               0x80000009
#define STATUS_RETURN_UNHANDLE_EXCEPTION        0x8000000A
#define STATUS_RETURN_NOT_SUPPORT               0x8000000B
#define STATUS_RETURN_CLIP_SUCCESS              0x8000000C

#define DRV_STATUS_LEVEL    0
#define LIB_STATUS_LEVEL    1
#define APP_STATUS_LEVEL    2

#define DRV_STATUS_LEVEL_NAME "Driver"
#define LIB_STATUS_LEVEL_NAME "Library"
#define APP_STATUS_LEVEL_NAME "Application"

#define STATUS_LEVEL_MASK       0x7f
#define STATUS_MODULE_MASK      0xff
#define STATUS_NO_MASK          0xffff

#define HX_STATUS_NUMBER( STATUS_LEVEL, MODULE, STATUS_NUMBER_IN_EACH_MODULE) ( 0x80000000 + ( (STATUS_LEVEL&STATUS_LEVEL_MASK)<<24 ) + ( (MODULE&STATUS_MODULE_MASK)<<16) + (STATUS_NUMBER_IN_EACH_MODULE&STATUS_NO_MASK) )

// ++++++++++++++++++++++++++++
// MACRO FUNCTION DECLARATIONS
// ++++++++++++++++++++++++++++

#if (defined(UART_CONSOLE))
#ifdef PROJECT_EPII
	#include "xprintf.h"
    #include "console_io.h"
    #define DbgDump(addr, size)
    #define DbgPrintMem(addr, size)
    #define DbgExit()
    #define DbgPrintf xprintf
    #define DbgPrintfSuppress(yes)
    #define uprintf xprintf
    #define uprint	xputs
#endif
#elif (defined(NO_CONSOLE))
    #define DbgDump(addr, size)
    #define DbgPrintMem(addr, size)
    #define DbgExit()
    #if HX_OS == HX_OS_WINDOWS
        #define DbgPrintf
        #define DbgPrint
    #else
        #define DbgPrintf(...)
        #define DbgPrint(lv, fmt, ...)
    #endif
#else
#error "com_debug.h : No Console Defined. Maybe you should define NO_CONSOLE"
#endif

#define SysLog DbgPrintf

#ifndef ErrPrintf
#define ErrPrintf DbgPrintf
#endif

#ifndef SuccessPrintf
#define SuccessPrintf DbgPrintf
#endif

#ifndef DbgPrintfSuppress
#define DbgPrintfSuppress(yes)
#endif

#ifndef DbgPrint
#define DbgPrint(lv, fmt, ...) \
    do { \
        if ( lv ) \
            DbgPrintf(fmt, ##__VA_ARGS__);\
    } while (0)
#endif

#define DbgPrintF DbgPrint

#undef ASSERT
#define ASSERT(arg) \
do { \
    if ( (arg) == 0 ) \
    { \
        DbgPrintf("ASSERT FAIL : %s %d \n",__FILE__,__LINE__);\
        return FALSE; \
    } \
} while (0)

// ++++++++++++++++++++++++++++
// DATA TYPE DECLARATIONS
// ++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++
// EXPORTED SUBPROGRAM SPECIFICATIONS
// ++++++++++++++++++++++++++++

#endif // _DEBUG_MESSAGE_INC_HEADER

