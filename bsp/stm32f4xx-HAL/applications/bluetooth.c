#include "bluetooth.h"
#include <rtthread.h>
#include "math.h"
#include <drivers/pin.h>
#include "string.h"
#include "easyflash.h"
#include "stdlib.h"
#include "stdio.h"
extern rt_device_t Bluewrite_device;
void _ansyBlueStatus(uint8_t * blueData);
void _initBlueStatus(uint8_t * blueData);
void input_blueTooth(uint8_t ch)
{
	static uint8_t blueFlag = BLUE_HEAD;
	static uint8_t blueStatus[BLUE_LEN];
	static uint8_t addCrc[2];
	static int revCount = 0;
	static int funCode = 0;
	short crc = 0;
	//rt_kprintf("%x\r\n",ch);
	switch(blueFlag)
	{
		case BLUE_HEAD:
			if(ch == BLUE_HEAD)
			{
				blueFlag = BLUE_FUNCODE;		
			}
			break;
		case BLUE_FUNCODE:
			if(ch == BLUE_FUNCODE || ch == BLUE_INITFUNCODE)
				blueFlag = BLUE_LEN;
			else
				blueFlag = BLUE_HEAD;
			funCode = ch;						
			break;
		case BLUE_LEN:
			if(ch == BLUE_LEN)
				blueFlag = BLUE_DATA;	
			else
				blueFlag = BLUE_HEAD;
			break;
		case BLUE_DATA:		
			blueStatus[revCount++] = ch;		
			if(revCount == BLUE_LEN)
			{
				blueFlag = BLUE_CRC;
				revCount = 0;
			}
			break;
		case BLUE_CRC:			
			addCrc[revCount++] = ch;
			if(revCount == 2)
			{
				blueFlag = BLUE_TITLE;
//				for(int i = 0 ; i < BLUE_LEN ; i ++)
//				{				
//					crc += blueStatus[i];
//				}		
//				if(addCrc[0] == (((short)(BLUE_FUNCODE + BLUE_LEN + crc))>>8) && (addCrc[1] == (((short)(BLUE_FUNCODE + BLUE_LEN + crc)) & 0x00FF)))
//				{
//					blueFlag = BLUE_TITLE;
//				}
//				else
//				{
//					blueFlag = BLUE_HEAD;
//				}
				revCount = 0;
				
			}	
			break;
		case BLUE_TITLE:
			if(ch == BLUE_TITLE)
			{
				
				//rev data finsh and set status
				if(BLUE_INITFUNCODE == funCode)
				{										
					_initBlueStatus(blueStatus);
				}
				else
				{
					_ansyBlueStatus(blueStatus);   //on the base of protocol 	
				}				
			}
			blueFlag = BLUE_HEAD;
			break;
	}
}


int s_initRevData[BLUE_LEN/2] = {0};
void _initBlueStatus(uint8_t * blueData)
{	
	for(int i = 0 ; i < BLUE_LEN/2 ; i++)
	{
		s_initRevData[i] = (blueData[i*2] << 8 | blueData[i*2+1]);
	}
	if(s_initRevData[1] > 0x8000)
	{
		s_initRevData[1] -= 65535;
	}
	rt_kprintf("init:%d\r\n",s_initRevData[1]);
}
static int MacnWorkStatus;
static int _MacnWorkStatus;
void _ansyBlueStatus(uint8_t * blueData)
{	
	short data = 0;
	MacnWorkStatus = 0;
	int revData[BLUE_LEN/2] = {0};
	for(int i = 0 ; i < BLUE_LEN/2 ; i++)
	{
		revData[i] = (blueData[i*2] << 8 | blueData[i*2+1]);
	}
	if(revData[1] > 0x8000)
	{
		revData[1] -= 65535;
	}
	//rt_kprintf("rev:%d\r\n",revData[1]);
	//clac macn work status	
	float diffSpeed = 0;
	if(revData[1] - s_initRevData[1] > 0)
	{
		diffSpeed = revData[1] - s_initRevData[1];		
		//a = 
		data = (short)((6*diffSpeed/(100-s_initRevData[1]))*10);
		MacnWorkStatus |=BLUE_FORWARD ;
		if(data > 60)
		{
			data = 60;
		}
		if(data < 0)
		{
			data = 0;
		}
	}
	else
	{
		diffSpeed = s_initRevData[1] - revData[1];		
		data = (short)((4*diffSpeed/(s_initRevData[1]))*10);		
		MacnWorkStatus |=BLUE_BACKUP ;
		if(data > 40)
		{
			data = 40;
		}
		if(data < 0)
		{
			data = 0;
		}		
	}
	 
	//char data = ((char)(0.147*pow(diffSpeed,1.1271))&0x0F);
	MacnWorkStatus |= data ;
	_MacnWorkStatus = MacnWorkStatus;
	//rt_kprintf("MacnWorkStatus:%x",MacnWorkStatus);
}

int getBlueMacnStatus()
{
	return _MacnWorkStatus;
}
int _get_Blue_CON()
{
	return rt_pin_read(53);
}
void _set_Blue_PWRC(int st)
{
	rt_pin_write(52,st);
}
void _set_Blue_At(int st)
{
	rt_pin_write(51,st);
}

//rt_device_write(write_device, 0,sendbuff,MACN_SEND_DATA_LEN);
void sendBuffer(uint8_t *buff)
{	
	rt_device_write(Bluewrite_device,0,buff,strlen((char *)buff));
}

void _sendCmdToBlue(uint8_t *buff,int len)
{	
	for(int i = 0 ; i < len ; i ++)
	{
		rt_kprintf(" %02x ",buff[i]);
	}
	rt_device_write(Bluewrite_device,0,buff,len);
}
void blueCmdSend(int cmd)
{
	//0 start
	//1 stop
	//6 init
	char buff[8] = {0};
	int i = 0 ;
	buff[i++] = 0x55;
	buff[i++] = cmd;
	buff[i++] = 0x01;	//len
	buff[i++] = 0x00;	//data
	buff[i++] = 0x01;	//crc	
	buff[i++] = 0xAA;	//title
	_sendCmdToBlue((uint8_t *)buff,i);
}

int initBlueSet()
{
	static int g_blueToothFalg = 0;
	g_blueToothFalg = 0;
	sendBuffer((uint8_t *)"AT+KBYTE3");
	rt_thread_delay(1000);
	if(_get_Blue_CON() == PIN_HIGH)
	{
		// had connect
		rt_kprintf("had connect\r\n");
		//sendBuffer((uint8_t *)"hello world\r\n");
		_set_Blue_At(PIN_HIGH);
		g_blueToothFalg = 1;
		return WXC_OK;
	}
	sendBuffer((uint8_t *)"AT+MAC");
	rt_thread_delay(1000);
	sendBuffer((uint8_t *)"AT+HOSTEN1");
	rt_thread_delay(1000);
	sendBuffer((uint8_t *)"AT+SCAN1");
	rt_thread_delay(3000);
	if(_get_Blue_CON() == PIN_HIGH)
	{
		rt_kprintf("had connect\r\n");
		g_blueToothFalg = 1;
		return WXC_OK;
	}
	sendBuffer((uint8_t *)"AT+RSLV");
	rt_thread_delay(1000);	
//	if(strlen(ef_get_env("blueCcid")) == 0)
//	{
//		sendBuffer((uint8_t *)"AT+RSLV");
//		rt_thread_delay(1000);
//	}
	uint32_t time = rt_tick_get();
//	while(strlen(ef_get_env("blueCcid")) == 0 && rt_tick_get() - time < 10000)
//	{
//		rt_thread_delay(300);
//	}
//	if(strlen(ef_get_env("blueCcid")) == 0)
//	{
//		//connect error
//		g_blueToothFalg = WXC_ERROR;
//		return g_blueToothFalg;
//	}
	char buff[100] = {0};
	sprintf(buff,"AT+BAND6AC2D2F21942\0");//ef_get_env("blueCcid")
	sendBuffer((uint8_t *)buff);
	rt_thread_delay(1000);
	memset(buff,0,100);
	sprintf(buff,"AT+CONNET6AC2D2F21942\0");//ef_get_env("blueCcid")
	sendBuffer((uint8_t *)buff);
	rt_thread_delay(1000);
	time = rt_tick_get();
	while(_get_Blue_CON() != PIN_HIGH && rt_tick_get() - time < 3000)
	{
		rt_thread_delay(500);
		rt_kprintf("wait connet\r\n");
	}
	if(_get_Blue_CON() != PIN_HIGH)
	{
		sendBuffer((uint8_t *)"AT+RSLV");
		rt_thread_delay(1000);
		return WXC_ERROR;
	}
	else
	{		
		rt_kprintf("had connect\r\n");
		return WXC_OK;
	}
	g_blueToothFalg = WXC_OK;
	return g_blueToothFalg;
}
void input_blueTooth_cmd(char ch)
{
	static char s_buff[64] = {0};
	static int s_index=0;
	s_buff[s_index++] = ch;
	if(ch == 0xAA)
	{
		s_index = 0;
	}
}


