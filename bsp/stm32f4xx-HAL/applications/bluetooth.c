#include "bluetooth.h"
#include <rtthread.h>
#include "math.h"
#include <drivers/pin.h>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "app.h"
#include "ansyBlueData.h"
static rt_device_t device; 
stuBodyExecInfo getBlueMacnStatus()
{
	return gBodyExecInfo;
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

void sendBuffer(uint8_t *buff)
{	
	rt_device_write(device,0,buff,strlen((char *)buff));
	rt_kprintf("%s",buff);
}
void _sendCmdToBlue(uint8_t *buff,int len)
{	
	for(int i = 0 ; i < len ; i ++)
	{
		rt_kprintf(" %02x ",buff[i]);
	}
	rt_device_write(device,0,buff,len);
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
	_sendCmdToBlue((uint8_t *)buff,i);
}
int initBlueSet(void)
{
	sendBuffer((uint8_t *)"AT+KBYTE3");
	rt_thread_delay(1000);
	if(_get_Blue_CON() == PIN_HIGH)
	{
		// had connect
		//rt_kprintf("had connect %d\r\n",_get_Blue_CON());
		//sendBuffer((uint8_t *)"hello world\r\n");
		_set_Blue_At(PIN_HIGH);
		return WXC_OK;
	}
	sendBuffer((uint8_t *)"AT+HOSTEN1");
	rt_thread_delay(1000);
	sendBuffer((uint8_t *)"AT+SCAN1");
	rt_thread_delay(3000);
	if(_get_Blue_CON() == PIN_HIGH)
	{
		//rt_kprintf("had connect\r\n");
		return WXC_OK;
	}
	sendBuffer((uint8_t *)"AT+RSLV");
	rt_thread_delay(1000);	
	uint32_t time = rt_tick_get();
	char buff[100] = {0};
	sprintf(buff,"AT+BAND%s\0",g_flashData.blueDevice);//ef_get_env("blueCcid")
	sendBuffer((uint8_t *)buff);
	rt_thread_delay(1000);
	memset(buff,0,100);
	sprintf(buff,"AT+CONNET%s\0",g_flashData.blueDevice);//ef_get_env("blueCcid")
	sendBuffer((uint8_t *)buff);
	rt_thread_delay(1000);
	time = rt_tick_get();
	while(_get_Blue_CON() != PIN_HIGH && rt_tick_get() - time < 30000)
	{
		rt_thread_delay(500);
		//rt_kprintf("wait connet\r\n");
	}
	if(_get_Blue_CON() != PIN_HIGH)
	{
		sendBuffer((uint8_t *)"AT+RSLV");
		rt_thread_delay(1000);
		return WXC_ERROR;
	}
	else
	{		
		//rt_kprintf("had connect\r\n");
		return WXC_OK;
	}
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

void initBlue(rt_device_t dev)
{
	device = dev;
}

void blueBuffWrite(char *buff,int len)
{
	rt_device_write(device,0,buff,len);
}
void blueBuffRead(char *buff,int *len)
{
	*len = rt_device_read(device, 0, buff,12);
}




static void _ansyBlueCmdData(char * revbuff,int len)
{
	rt_kprintf("%s",revbuff);
}
void input_blueTooth_cmd(char ch)
{
	static char s_buff[64] = {0};
	static int s_index=0;
	s_buff[s_index++] = ch;
	if(ch == 0xAA)
	{
		//had end rev data
		s_buff[s_index] = '\0';
		_ansyBlueCmdData(s_buff,s_index);
		memset(s_buff,0,s_index);
		s_index = 0;		
	}
}


