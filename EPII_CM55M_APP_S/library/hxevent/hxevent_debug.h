#ifndef _HXEVENT_DEBUG_H_
#define _HXEVENT_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARM_PRINTF
	#ifdef LIB_COMMON
		#include "xprintf.h"
		#define HXEVENT_PRINTF xprintf
	#else
		#include <stdio.h>
		#define HXEVENT_PRINTF printf
	#endif
#endif


#if defined(DEBUG)
#if defined(DEBUG_HOSTLINK)
#include <stdio.h>
#define DBG(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define DBG(fmt, ...)	HXEVENT_PRINTF(fmt, ##__VA_ARGS__)
#endif
#else
#define DBG(fmt, ...)
#endif

#define DBG_LESS_INFO	0x01    /* less debug  messages */
#define DBG_MORE_INFO	0x02    /* more debug  messages */


#if defined (DBG_LESS)
#if defined (OMRON_FD_VIP_BD) && defined(OMRON_FD_FR)
#define DBG_TYPE		0
#else
#define DBG_TYPE		(DBG_LESS_INFO)
#endif
#elif defined (DBG_MORE)
#define DBG_TYPE		((DBG_LESS_INFO) | (DBG_MORE_INFO))
#else
#define DBG_TYPE 		0
#endif

#if DBG_TYPE > 0
#define dbg_printf(type, fmt, ...) \
		if (((type) & DBG_TYPE))  { HXEVENT_PRINTF(fmt, ##__VA_ARGS__); }
#else
#define dbg_printf(type, fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* HXEVENT_DEBUG_H */
