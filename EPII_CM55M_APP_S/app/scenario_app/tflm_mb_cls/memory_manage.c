/*
 * memory_manage.c
 *
 *  Created on: Feb 15, 2023
 *      Author: bigcat-himax
 */
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static uint32_t g_addr=0, end_addr=0;

int mm_set_initial(uint32_t start_addr, uint32_t mm_sz)
{
	g_addr = start_addr;
	end_addr = g_addr + mm_sz;
	//xprintf("mm_set_initial:\n start[%x]end[%x]sz[%x]\n", start_addr,start_addr+mm_sz,mm_sz);

	return 0;
}

uint32_t mm_reserve(uint32_t sz)
{
	uint32_t res = g_addr;
	if((g_addr+sz)>end_addr)
		return 0;
	else {
		g_addr += sz;
		return res;
	}

}

uint32_t mm_reserve_align(uint32_t sz, uint32_t alignment_size)
{
	//uint32_t res = g_addr&(alignment_size - 1U) == 0?g_addr:g_addr&~(alignment_size - 1U)+alignment_size;
	uint32_t res = g_addr&~(alignment_size - 1U);
	if(res!=g_addr)
		res+=alignment_size;
	
	if((res+sz)>end_addr)
		return 0;
	else {
		g_addr = res+sz;
		return res;
	}

}
