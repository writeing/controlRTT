#include "bluetooth.h"
#include <rtthread.h>
#include "math.h"
#include <drivers/pin.h>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "app.h"
#include "ansyBlueData.h"
#include "at.h"
#include "blueOperation.h"
//blue send and rev obj
static rt_device_t device;

stuBodyExecInfo getBlueMacnStatus()
{
	return gBodyExecInfo;
}
int _get_Blue_CON()
{
	return rt_pin_read(BLE_CON);
}
void _set_Blue_PWRC(int st)
{
	rt_pin_write(BLE_PWRC,st);
}
void _set_Blue_At(int st)
{
	rt_pin_write(BLE_AT,st);
}
//send data to blue
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
	g_stublueOpt.write(buff,i);
//	_sendCmdToBlue((uint8_t *)buff,i);
}

typedef struct blueCmdList
{
	char buff[20];
	char flag;
	char linenum;
	char timeOutS;
	char *parm;
}stuBleCmdList;

static stuBleCmdList g_blecmdList[] = 
{
	{"AT+KBYTE3",	RESP_STATUS_KEEP,	2,	1,	RT_NULL},
	{"AT+HOSTEN1",	RESP_STATUS_KEEP,	2,	3,	RT_NULL},
	{"AT+SCAN1",	RESP_STATUS_KEEP,	2,	1,	RT_NULL},
	{"AT+RSLV",		RESP_STATUS_KEEP,	2,	1,	RT_NULL},
	{"AT+BAND",		RESP_STATUS_DELETE,	3,	30,	g_flashData.blueDevice},
	{"AT+CONNET",	RESP_STATUS_DELETE,	2,	30,	g_flashData.blueDevice},	
};

int setBlueInit(void)
{
	static int sendindex = 1;
	char buff[100] = {0};
	int readLen = 0;
	if(sendindex == ARRAY_SIZE(g_blecmdList))
	{
		return 0;
	}
//	for(int i = 0 ; i < ARRAY_SIZE(g_blecmdList) ; i ++)
//	{
		//check ble has connect
		if(_get_Blue_CON() == PIN_HIGH)
			return RT_EOK;
		rt_kprintf("%s\r\n",g_blecmdList[sendindex].buff);
		g_stublueOpt.write(g_blecmdList[sendindex].buff,rt_strlen((char *)g_blecmdList[sendindex].buff) - 1);
		rt_thread_delay(1000);
		sendindex++;

//		g_stublueOpt.read(buff,&readLen);
//		//getRevDataForKey("OK",buff);
//		rt_kprintf("%s\r\n",buff);
//	}
//	rt_uint32_t time = rt_tick_get();
//	while(_get_Blue_CON() != PIN_HIGH && rt_tick_get() - time < 30000)
//	{
//		rt_thread_delay(500);
//	}
//	if(_get_Blue_CON() != PIN_HIGH)
//	{
//		rt_thread_delay(1000);
//		return WXC_ERROR;
//	}
//	else
//	{		
//		rt_kprintf("had connect\r\n");
//		return WXC_OK;
//	}
}
int getBlueConnectStatus()
{
	if(_get_Blue_CON() == PIN_HIGH)
	{
		return 1;
	}
	else
		return 0;
}
/***************blue rev and send func******************/

//
void initOldBle(rt_device_t bledevice)
{
	device = bledevice;
	_set_Blue_At(PIN_LOW);
}
void setBle(void)
{
	g_stublueOpt.write("AT+HOSTEN1",10);
}
void blueBuffWrite(char *buff,int len)
{
	rt_device_write(device,0,buff,len);
}
void blueBuffRead(char *buff,int *len)
{
	*len = rt_device_read(device, 0, buff,*len);
}
/****************************************************/


