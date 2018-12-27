/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>
#include "easyflash.h"
#include "app.h"
#include "dac.h"
int main(void)
{ 
	MX_DAC_Init();
	
    /* user app entry */
//	rt_err_t result;
	while(1)
	{
		rt_thread_delay(100);
		//rt_kprintf("time");
	}	
//    return 0;
}
