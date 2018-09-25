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
int main(void)
{
    /* user app entry */
	easyflash_init();
	ef_load_env();
	ef_set_and_save_env("wxc","123456");
	while(1)
	{
		//rt_kprintf("hello world %s\r\n",ef_get_env("wxc"));
		rt_thread_delay(100);		
	}	
    return 0;
}
