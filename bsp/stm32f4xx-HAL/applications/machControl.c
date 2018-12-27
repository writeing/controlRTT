#include "machControl.h"
#include <rtthread.h>
#include "dac.h"
stuMacnControl g_stumach;
void _setFucode(char funcode);

void _openMacn()
{
	g_stumach.FLag = MACN_BEGIN;
}
void _closeMacn()
{
	g_stumach.FLag = MACN_END;
}
char _checkMacn()
{
	return g_stumach.FLag;
}
void initMacninfo(int FuncCode)
{
	_setFucode(FuncCode);
	g_stumach.sendbuff[0] = MACH_HEAD;
	g_stumach.sendbuff[1] = 0x01;
	g_stumach.sendbuff[2] = MACH_FUNCODE;
	g_stumach.sendbuff[3] = MACH_LEN;
	g_stumach.sendbuff[8] = MACH_TITLE;
	setSendData(0,0);	
	_openMacn();
}
void _setFucode(char funcode)
{
	g_stumach.FuncCode = funcode;	
}
void setSendData(char data1,char data2)
{
	g_stumach.sbuff[0] = data1;
	g_stumach.sbuff[1] = data2;

}
int getMacnRunStatus()
{
	return g_stumach.FLag;
}
void _getCrcData()
{
	short crc = 0;
	crc+=0x01;
	crc+=g_stumach.sendbuff[1];
	crc+= g_stumach.sendbuff[3];
	crc+=g_stumach.sbuff[0];
	crc+=g_stumach.sbuff[1];
	g_stumach.crc = crc;
}
char *getSendData()
{
	_getCrcData();
	g_stumach.sendbuff[4] = g_stumach.sbuff[0];
	g_stumach.sendbuff[5] = g_stumach.sbuff[1];
	g_stumach.sendbuff[6] = g_stumach.crc & 0x00FF;
	g_stumach.sendbuff[7] = (g_stumach.crc & 0xFF00) >> 8;
	return g_stumach.sendbuff;
}
void ansyMacnStatus(char sta)
{
	_closeMacn();
	setSendData(0,0);
	//发生了异常，准备重新工作
//	rt_thread_delay(1000);
//	setSendData(1,1);
//	
//	_openMacn();
}
void checkMacnData(uint8_t ch)
{
	static uint8_t macnFlag = MACH_HEAD;
	static uint8_t macnStatus[2];
	static uint8_t addCrc[2];	
	static int revCount = 0;
	short crc = 0;
	switch(macnFlag)
	{
		case MACH_HEAD:
			if(ch == MACH_HEAD)
			{
				macnFlag = MACH_ADDRESS;			
			}			
			break;
		case MACH_ADDRESS:
			if(ch == MACH_ADDRESS)
			{
				macnFlag = MACH_FUNCODE;			
			}			
			break;
		case MACH_SEND_FUNCODE:
			if(ch == MACH_FUNCODE)
				macnFlag = MACH_LEN;	
			else
				macnFlag = MACH_HEAD;
			break;
		case MACH_LEN:
			if(ch == MACH_LEN)
				macnFlag = MACH_DATA;	
			else
				macnFlag = MACH_HEAD;
			break;
		case MACH_DATA:	
			macnStatus[revCount++] = ch;				
			if(revCount == MACH_LEN)
			{
				macnFlag = MACH_CRC;
				revCount = 0;
			}
			break;
		case MACH_CRC:	
			addCrc[revCount] = ch;			
			if(revCount == 2)
			{
				macnFlag = MACH_TITLE;
//				for(int i = 0 ; i < MACH_LEN ; i ++)
//				{
//					crc += macnStatus[i];
//				}
//				if(addCrc[0] == ((short)(MACH_FUNCODE + MACH_LEN + crc))>>8 && (addCrc[1] == (((short)(MACH_FUNCODE + MACH_LEN + crc)) & 0x00FF)))
//				{
//					macnFlag = MACH_TITLE;
//				}
//				else
//				{
//					macnFlag = MACH_HEAD;
//				}
				revCount = 0;
			}
			
			break;
		case MACH_TITLE:
			if(ch == MACH_TITLE)
			{
				//rev data finsh and set status
				//todo:macnStatus
				if(macnStatus[0] == 0x02)
				{
					ansyMacnStatus(macnStatus[1]);   //on the base of protocol 
				}
				if(macnStatus[0] != 0x02 && _checkMacn() == MACN_END)
				{
					_openMacn();		//had ok					
				}
			}
			macnFlag = MACH_HEAD;
			break;
	}
}


void setRightjoytick(int machdire,int speed)
{
	if(machdire == MACN_FORWARD)
	{
		if(10 <= speed && speed <= 40)
		{
			speed = (int)(((409.6/40)*speed) + 2048*1.3) - 1;
		}
		else if(speed > 40 && speed <= 60)
		{
			speed = (int)((51.2*(speed-40)) + 2048*1.5) - 1;
		}
		goto run;
	}
	else if(machdire == MACN_BACKUP)
	{
		speed = 2048-(int)(2048/30)*speed;
		goto run;
	}
	else if(machdire == MACN_STOP)
	{
		speed = 2048;
		//end
		HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,speed);
		HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
		HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,speed);
		HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
		return;
	}
	else if(machdire == MACN_LEFT)
	{
		speed = 0;
	}
	else if(machdire == MACN_RIGHT)
	{
		speed = 4096;
	}
	rt_kprintf("macndire = %d,speed = %d\r\n",machdire,speed);
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,speed);
	HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
	return;	
	run:
	rt_kprintf("macndire = %d,speed = %d\r\n",machdire,speed);
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,speed);
	HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
	return;
	
}
void setMacbWorkStatus(int status)
{
	uint8_t macndire = status >> 16;
	uint8_t macnSpeed = status & 0x0FFFF;
	
	setRightjoytick(macndire,macnSpeed);
	switch(macndire)
	{
		case MACN_FORWARD:
			setSendData(1,macnSpeed);
			break;
		case MACN_BACKUP:
			setSendData(1,(macnSpeed*-1));
			break;
		case MACN_LEFT:
			setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;
		case MACN_RIGHT:
			setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;		
		case MACN_STOP:
			setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;			
	}	
}


