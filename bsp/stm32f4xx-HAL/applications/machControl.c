#include "machControl.h"
#include <rtthread.h>
stuMacnControl g_stumach;
void _setFucode(char funcode);
void initMacninfo(int FuncCode)
{
	_setFucode(FuncCode);
	g_stumach.sendbuff[0] = g_stumach.HEAD;
	g_stumach.sendbuff[1] = g_stumach.FuncCode;
	g_stumach.sendbuff[2] = g_stumach.dataLen;
	g_stumach.sendbuff[6] = g_stumach.title;
	setSendData(0,0);	
	g_stumach.FLag = MACN_BEGIN;
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
	unsigned char crc = 0;
	crc+=g_stumach.FuncCode;
	crc+= g_stumach.dataLen;
	crc+=g_stumach.sbuff[0];
	crc+=g_stumach.sbuff[1];
	g_stumach.crc = crc;
}
char *getSendData()
{
	_getCrcData();
	g_stumach.sendbuff[3] = g_stumach.sbuff[0];
	g_stumach.sendbuff[4] = g_stumach.sbuff[1];
	g_stumach.sendbuff[5] = g_stumach.crc;
	return g_stumach.sendbuff;
}
void ansyMacnStatus(char sta)
{
	//g_stumach.FLag = MACN_BEGIN;
	//if(sta == )
}
void checkMacnData(char ch)
{
	static uint8_t macnFlag = MACH_HEAD;
	static uint8_t macnStatus[2];
	static int revCount = 0;
	switch(macnFlag)
	{
		case MACH_HEAD:
			if(ch == MACH_HEAD)
			{
				macnFlag = MACH_FUNCODE;			
			}			
			break;
		case MACH_FUNCODE:
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
			if(revCount == MACH_LEN)
			{
				macnFlag = MACH_CRC;
				revCount = 0;
			}
			else
			{
				macnStatus[revCount++] = ch;				
			}			
			break;
		case MACH_CRC:
			if(ch == ((uint8_t)(MACH_FUNCODE + MACH_LEN + macnStatus[0] + macnStatus[1])))
			{
				macnFlag = MACH_TITLE;
			}
			else
			{
				macnFlag = MACH_HEAD;
			}
			break;
		case MACH_TITLE:
			if(ch == MACH_TITLE)
			{
				//rev data finsh and set status
				//todo:macnStatus
				ansyMacnStatus(macnStatus[0]);   //on the base of protocol 
			}
			macnFlag = MACH_HEAD;
			break;
	}
}
void setMacbWorkStatus(uint8_t status)
{
	uint8_t macndire = status >> 4;
	uint8_t macnSpeed = status & 0x0F;
	switch(macndire)
	{
		case MACN_FORWARD:
			setSendData(macndire,macnSpeed);
			break;
		case MACN_BACKUP:
			setSendData(macndire,macnSpeed);
			break;
		case MACN_LEFT:
			setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;
		case MACN_RIGHT:
			setSendData(0,0);
			//setSendData(macndire,macnSpeed);
			break;		
	}	
}


