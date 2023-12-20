/*****************************************************************************
 * MODULE NAME: Com_def.h
 * PURPOSE:  Common type or data structure header file
 *
 * NOTES:
 *
 * CREATOR: roc Tsay (11/25/2004)
 * Revision Log:
 * Copyright(C) Himax Technology, 2004
 ******************************************************************************/

#ifndef _COMMON_DEF_INC_HEADER
#define _COMMON_DEF_INC_HEADER

// ++++++++++++++++++++++++++++
// NAMING CONSTANT DECLARATIONS
// ++++++++++++++++++++++++++++

#ifndef NULL
// define my local NULL definition
#define NULL ((void *)0)
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef unsigned char BOOL_T;

/*indicates that the parameter is not used (for expansion)*/
#define NOUSED(_v) ((void)(_v))

/*The larger of __x and __y*/
//JACKY mark
//#define MAX(__x,__y) ((__x)>(__y)?(__x):(__y))
//JACKY mark
/*The smaller of __x and __y*/
//#define MIN(__x,__y) ((__x)<(__y)?(__x):(__y))
typedef void VOID;
typedef void *PVOID;

typedef unsigned char UCHAR_T;
typedef unsigned char UINT8_T;
typedef unsigned short UINT16_T;
typedef unsigned int UINT32_T;

typedef char CHAR_T;
typedef signed char INT8_T;
typedef short INT16_T;
typedef int INT32_T;
typedef int STATUS_T;

typedef unsigned long long UINT64_T;
typedef long long INT64_T;

#ifdef __CC_ARM
#define INLINE __inline
#else
#define INLINE inline
#endif

#ifdef MODULE
typedef unsigned long ULONG_T;
#else
typedef unsigned int ULONG_T;
#endif // MODULE

#ifdef MODULE
    #include <linux/types.h>
#else
#include <stddef.h>
#endif // MODULE

// ++++++++++++++++++++++++++++
// MACRO FUNCTION DECLARATIONS
// ++++++++++++++++++++++++++++
#define HiByte16(val) (val>>8)
#define LoByte16(val) (val&0xff)

#define HiByte32(val) (val>>24)
#define H2Byte32(val) ((val>>16) & 0xff)
#define H3Byte32(val) ((val>>8) & 0xff)
#define LoByte32(val) (val&0xff)

#define HiByte16_p(p) (((UINT8_T*)(p)))
#define LoByte16_p(p) (((UINT8_T*)(p))+1)

#define HiByte32_p(p) (((UINT8_T*)(p))+3)
#define H2Byte32_p(p) (((UINT8_T*)(p))+2)
#define H3Byte32_p(p) (((UINT8_T*)(p))+1)
#define LoByte32_p(p) (((UINT8_T*)(p)))

// ++++++++++++++++++++++++++++
// DATA TYPE DECLARATIONS
// ++++++++++++++++++++++++++++

//  -------------------  PACKED -----------------------
#ifdef __GNUC__
#pragma pack(1)
#endif

#ifdef __GNUC__
#pragma pack()
#endif
//  -------------------  UNPACKED ---------------------

// ++++++++++++++++++++++++++++
// EXPORTED SUBPROGRAM SPECIFICATIONS
// ++++++++++++++++++++++++++++

#endif // _COMMON_DEF_INC_HEADER

