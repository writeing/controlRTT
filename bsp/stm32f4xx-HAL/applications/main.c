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
#include "app.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "flash.h"
#include "bluetooth.h"
#include "blueApp.h"
#include "HAL_adc.h"
#include "HAL_pwm.h"

#define FLASH_DATA_ADDRESS                  0x0801A000

stuFlashData g_flashData;
void printCharDataToFlash(char pdata,char *name)
{
	char buff[10];
	sprintf(buff,"%d",pdata);
	
}
void printStrDataToFlash(char *pdata,char *name)
{
	
}
void saveFalshData()
{
	if(STMFLASH_WriteStruct(FLASH_DATA_ADDRESS,&g_flashData,sizeof(stuFlashData)) == HAL_ERROR)
	{		
		rt_kprintf("flash write false");
	}
}
void initFlash()
{
	STMFLASH_ReadStruct(FLASH_DATA_ADDRESS,&g_flashData,sizeof(stuFlashData));
	if(g_flashData.qMaxSpeed == 0xFF || g_flashData.qMaxSpeed == 0)
	{
		// don`t init flash user default value
		g_flashData.qMaxSpeed = 60;
		g_flashData.bodyAngle = 20;
		g_flashData.hMaxSpeed = 30;
		char buff[20] = "6AC2D2F21942";
		memcpy(g_flashData.blueDevice,buff,20);
		g_flashData.runmode = 1;
		g_flashData.stoprun = 5;
		g_flashData.stoprun = 5;
		g_flashData.houJiaoDu = 20;		
	}	
	//rt_kprintf("init flash complete\r\n");
//	saveFalshData();
}
void initExternFlash(void)
{
	MX_TIM3_Init();
	MX_ADC1_Init();	
	//init flash data
	initFlash();
}

int main(void)
{ 

	uint32_t channel1,channel2;
    /* user app entry */
//	rt_err_t result;

	//后面放到总的初始化地方，在这个线程启动之后的地方	
	rt_thread_delay(300);
	beginSendBlueCmd();
	initBlueSet();		  //
	beginRevBlueData(); 
	
	while(1)
	{
		rt_thread_delay(50);
		//foreach get adc data
		//AdcGetValue(&channel1,&channel2);
		//exec adc value 
		//send data to blue 		
	}	
//    return 0;
}






