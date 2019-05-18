/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>

int main(void)
{
    /* user app entry */
	for(;;)
	{
		//rt_kprintf("hello\r\n");
		rt_thread_delay(100);	
	}
    return 0;
}
