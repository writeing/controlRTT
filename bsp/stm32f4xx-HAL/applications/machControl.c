#include "machControl.h"
#include <rtthread.h>
#include "dac.h"
#include "app.h"
#include "debugUsart.h"
stuMacnControl g_stumach;

int g_nowSpeed = 0;
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
	g_stumach.sendbuff[2] = 0x03;
	g_stumach.sendbuff[3] = 0x02;
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
	//根据sta 的值确定是过温，还是过压
	
	//发生异常之后，不做任何操作，并且停止工作
}
void checkMacnData(uint8_t ch)
{	
	static uint8_t macnFlag = MACH_HEAD;
	static uint8_t macnStatus[4];
	static uint8_t addCrc[2];	
	static int revCount = 0;
	static int crc = 0;
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
			else
			{
				macnFlag = MACH_HEAD;
			}
			break;
		case MACH_FUNCODE:
			if(ch == 0x01)
			{
				macnFlag = MACH_LEN;	
				crc += ch;
			}
			else
			{
				macnFlag = MACH_HEAD;
				crc = 0;
			}
			break;
		case MACH_LEN:
			if(ch == 0x04)
			{
				macnFlag = MACH_DATA;	
				crc += ch;
			}
			else
			{
				macnFlag = MACH_HEAD;
				crc = 0;
			}
			break;
		case MACH_DATA:	
			macnStatus[revCount++] = ch;
			crc += ch;
			if(revCount == 0x04)
			{
				macnFlag = MACH_CRC;
				revCount = 0;
			}
			break;
		case MACH_CRC:	
			addCrc[revCount++] = ch;			
			if(revCount == 2)
			{
				if(addCrc[0] == ((crc & 0xFF00) >> 8) && addCrc[0] == (crc &0x00FF))
				{
					macnFlag = MACH_TITLE;
				
				}
				else
				{
					macnFlag = MACH_HEAD;
				}
				revCount = 0;	
				crc = 0;
			}
			break;
		case MACH_TITLE:
			if(ch == 0xAA)
			{
				//rev data finsh and set status
				//todo:macnStatus
				if(macnStatus[0] > 0x01)
				{
					// error
					ansyMacnStatus(macnStatus[1]);   //on the base of protocol 					
				}
				if(macnStatus[0] < 2 && _checkMacn() == MACN_END)
				{
					_openMacn();		//had ok			
				}				
				sendSignalDataToPc(macnStatus[2],macnStatus[3]);
				g_nowSpeed = (int)((macnStatus[2] << 8) | macnStatus[3]);
			}
			macnFlag = MACH_HEAD;
			break;
	}
}

void checkMachStatus(rt_device_t machUsart)
{
	if(getMacnRunStatus() != MACN_BEGIN)
	{
		//send stop cmd
		deviceStatus = DEVICE_END;
	}	
}
void setRunjoytick(int machdire,int speed)
{
	static int count = 0;
//	int t_speed = speed;
	if(machdire == MACN_FORWARD)
	{
		speed = 2048+(int)(2048/g_flashData.qMaxSpeed)*speed - 1;
	}
	else if(machdire == MACN_BACKUP)
	{
		speed = 2048-(int)(2048/g_flashData.hMaxSpeed)*speed;
	}
	else if(machdire == MACN_STOP)
	{
		speed = 2048;
		//end
		HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,speed);
		HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
		return;
	}
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,speed);
	HAL_DAC_Start(&hdac,DAC_CHANNEL_1);	
	if((++count)% DEVICE_BLUE_PRINT_DATA_INTERVAL == 0)
	{
		rt_kprintf("run macndire = %d,speed = %d\r\n",machdire,speed);
		count = 0;
	}
	return;
	
}
void setRightjoytick(int machdire,int speed)
{
	static int count = 0;
//	int t_speed = speed;
	if(machdire == MACN_RIGHT)
	{
		speed = 2048+(int)(2048/60)*speed - 1;
	}
	else if(machdire == MACN_LEFT)
	{
		speed = 2048-(int)(2048/60)*speed;
	}
	if((++count)% DEVICE_BLUE_PRINT_DATA_INTERVAL == 0)
	{
		rt_kprintf("right macndire = %d,speed = %d\r\n",machdire,speed);
		count = 0;
	}
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,speed);
	HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
	return;
	
}
void setCebodyjoytick(int machdire,int speed)
{
	static int count = 0;
//	int t_speed = speed;
	if(machdire == MACN_CERIGHT)
	{
		speed = 2048+(int)(2048/60)*speed - 1;
	}
	else if(machdire == MACN_CELEFT)
	{
		speed = 2048-(int)(2048/60)*speed;
	}
	if((++count)% DEVICE_BLUE_PRINT_DATA_INTERVAL == 0)
	{
		rt_kprintf("cebody macndire = %d,speed = %d\r\n",machdire,speed);
		count = 0;
	}
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,speed);
	HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
	return;
	
}


void setMacbWorkStatus(stuBodyExecInfo status)
{

	uint8_t macndire;
	uint8_t macnSpeed;
#if DEVICE_RIGHT_BODY_DATA == HAL_OK
	macndire = status.right >> 16;
	macnSpeed = (status.right) & 0x0FFFF;
	setRightjoytick(macndire,macnSpeed);
#endif

#if DEVICE_CE_BODY_DATA == HAL_OK	
	macndire = status.ceBody >> 16;
	macnSpeed = (status.ceBody) & 0x0FFFF;
	setCebodyjoytick(macndire,macnSpeed);	
#endif
	
#if DEVICE_RUN_BODY_DATA == HAL_OK	
	macndire = status.run >> 16;
	macnSpeed = (status.run) & 0x0FFFF;
	setRunjoytick(macndire,macnSpeed);	
#endif
	
	switch(macndire)
	{
		case MACN_FORWARD:
			setSendData(1,macnSpeed);
			break;
		case MACN_BACKUP:
			setSendData(1,(macnSpeed*-1));
			break;
		case MACN_LEFT:
			//setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;
		case MACN_RIGHT:
			//setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;		
		case MACN_STOP:
			setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;	
		case MACN_CELEFT:
			//setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;	
		case MACN_CERIGHT:
			//setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;				
	}	
}


