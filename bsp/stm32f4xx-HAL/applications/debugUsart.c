#include "bluetooth.h"
#include <rtthread.h>
#include "math.h"
#include <drivers/pin.h>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "debugUsart.h"
#include "string.h"
#include "shell.h"
extern struct finsh_shell *shell;

void cleanBuff(char * buff)
{
	memset(buff,0,REV_BUFF_COUNT);
}
void sendAllDataToPc()
{
	char sbuff[30] = {0};
	int index = 0;
	sbuff[index++] = SEND_DEVICE_ID;
	sbuff[index++] = SEND_DATA_MODE;
	sbuff[index++] = 0x04*2;
	//run mode
	sbuff[index++] = 0x00;
	sbuff[index++] = g_flashData.runmode;	
	// stop data
	sbuff[index++] = g_flashData.stoptui;
	sbuff[index++] = g_flashData.stoprun;	
	// q max speed 
	sbuff[index++] = 0x00;
	sbuff[index++] = g_flashData.qMaxSpeed;
	// hou info
	sbuff[index++] = g_flashData.houJiaoDu;
	sbuff[index++] = g_flashData.hMaxSpeed;
	//crc
	sbuff[index++] = 0x00;
	sbuff[index++] = 0x00;	
	rt_device_write(shell->device, 0,sbuff,index);
	
}
void sendSignalDataToPc(char data1,char data2)
{
	char sbuff[8] = {0};	
	int index = 0;
	sbuff[index++] = SEND_DEVICE_ID;
	sbuff[index++] = SEND_DATA_MODE;
	sbuff[index++] = 0x02;//len
	sbuff[index++] = data1 ;//data1
	sbuff[index++] = data2;//data2
	sbuff[index++] = 0 ;//crc1
	sbuff[index++] = 0 ;//crc2
	rt_device_write(shell->device, 0,sbuff,index);
}
void getCmdData(char *buff,int index)
{
	int data[2] = {0,0};
	int cmd = (buff[REV_CMD_INDEX_HIGH] << 8 )| buff[REV_CMD_INDEX_LOW];	
//			int data =(buff[REV_DATA_INDEX_HIGH] << 8 )| buff[REV_DATA_INDEX_LOW]; 	
	//rt_kprintf("cmd = %d\r\n",cmd);
	switch (cmd)
		{
		case REV_CMD_TYPE_RUN_MODE:
			data[0] = buff[REV_DATA_INDEX_LOW];
			g_flashData.runmode = data[0];
			break;
		case REV_CMD_TYPE_STOP_DATA:
			data[0] = buff[REV_DATA_INDEX_HIGH];			
			data[1] = buff[REV_DATA_INDEX_LOW];		
			g_flashData.stoptui = data[0];
			g_flashData.stoprun = data[1];			
			break;
		case REV_CMD_TYPE_RUN_SPEED:
			data[0] = buff[REV_DATA_INDEX_LOW];
			g_flashData.qMaxSpeed = data[0];
			break;
		case REV_CMD_TYPE_HOUTUI_INFO:
			data[0] = buff[REV_DATA_INDEX_HIGH];			
			data[1] = buff[REV_DATA_INDEX_LOW];	
			g_flashData.houJiaoDu = data[0];
			g_flashData.hMaxSpeed = data[1];			
			break;
		case REV_CMD_TYPE_RIGHT_INFO:
			data[0] = buff[REV_DATA_INDEX_HIGH];			
			data[1] = buff[REV_DATA_INDEX_LOW];	
			break;
		case REV_CMD_TYPE_ZHUAN_INFO:
			data[0] = buff[REV_DATA_INDEX_HIGH];			
			data[1] = buff[REV_DATA_INDEX_LOW];		
			break;
		case REV_CMD_TYPE_GET_INFO:
			//get info to pc
			sendAllDataToPc();
			return;
		default:return;
		}
		sendSignalDataToPc(0,0);
		//save data to flash
		saveFalshData();
}
void debugAnsy(char ch)
{
	static char revStauts = 0;
	static char s_revBuff[REV_BUFF_COUNT] = {0};
	static int s_index = 0;
	if(ch == REV_DEVICE_ID && s_index == 0)
	{
		revStauts = 1;
	}	
	//rt_kprintf("rev = %d,%d,%x\r\n",s_index,revStauts,ch);
	s_revBuff[s_index++] = ch;
	if(revStauts == 1)
	{
		if(s_index == 8)
		{
			//rt_kprintf("rev success\r\n");
			getCmdData(s_revBuff,s_index);
			memset(s_revBuff,0,REV_BUFF_COUNT);
			s_index = 0;
			revStauts = 0;
		}
	}
	if(s_index == REV_BUFF_COUNT)
	{
		s_index = 0;
	}	
	if(strstr(s_revBuff,"begin")!= 0)
	{
		rt_kprintf("begin");
		memset(s_revBuff,0,REV_BUFF_COUNT);
		s_index = 0;
	}
	if(strstr(s_revBuff,"ccid")!= 0)
	{
		char * ccid = strtok(s_revBuff,":");
		ccid = strtok(NULL,":");
		memcpy(g_flashData.blueDevice,ccid ,strlen(ccid));
		rt_kprintf("ccid = %s\r\n",g_flashData.blueDevice);
		sendSignalDataToPc(0,0);
		memset(s_revBuff,0,REV_BUFF_COUNT);
		s_index = 0;
	}
}


