/*
 * WE2_debug.h
 *
 *  Created on: 2021/03/27
 *      Author: 902447
 */

#ifndef EPII_DEBUG_H_
#define EPII_DEBUG_H_

#define	 DBG_LESS

#define DBG_LESS_INFO	0x01    /* less debug  messages */
#define DBG_MORE_INFO	0x02    /* more debug  messages */

#ifndef WE2_APP_PRINTF
#ifdef LIB_COMMON
		#include "xprintf.h"
		#define WE2_APP_PRINTF xprintf
	#else
#include <stdio.h>
#define WE2_APP_PRINTF printf
#endif
#endif

#if defined (DBG_LESS)
#define DBG_TYPE		(DBG_LESS_INFO)
#elif defined (DBG_MORE)
#define DBG_TYPE		((DBG_LESS_INFO) | (DBG_MORE_INFO))
#else
#define DBG_TYPE 		0
#endif

#if DBG_TYPE > 0
#define dbg_printf(type, fmt, ...) \
		if (((type) & DBG_TYPE))  { WE2_APP_PRINTF(fmt, ##__VA_ARGS__); }
#else
#define dbg_printf(type, fmt, ...)
#endif

#endif /* EPII_DEBUG_H_ */
