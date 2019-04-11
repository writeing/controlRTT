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

static at_response_t arp = RT_NULL;
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
	sendDataToBle(buff,i);
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

int initBlueSet(void)
{
	char buff[100] = {0};
	for(int i = 0 ; i < ARRAY_SIZE(g_blecmdList) ; i ++)
	{
		//check ble has connect
		if(_get_Blue_CON() == PIN_HIGH)
			return RT_EOK;
		//check resp is del
		if(!arp)
		{
			setBlueRevParm(g_blecmdList[i].linenum ,g_blecmdList[i].timeOutS * 1000);			
		}
		//check pram is NULL
		if(g_blecmdList[i].parm != RT_NULL)
		{		
			memset(buff,0,100);
			sprintf(buff,"%s%s",g_blecmdList[i].buff,g_blecmdList[i].parm);//ef_get_env("blueCcid")
			sendBlueCmdData(g_blecmdList[i].buff,g_blecmdList[i].flag);		
		}
		else
		{
			sendBlueCmdData(g_blecmdList[i].buff,g_blecmdList[i].flag);	
		}
		//get return line
		at_resp_parse_line_args(arp,1,"%s",buff);
		//getRevDataForKey("OK",buff);
		rt_kprintf("%s",buff);
	}
	rt_uint32_t time = rt_tick_get();
	while(_get_Blue_CON() != PIN_HIGH && rt_tick_get() - time < 30000)
	{
		rt_thread_delay(500);
	}
	setBlueRevParm(0,1000);
	if(_get_Blue_CON() != PIN_HIGH)
	{
		sendBlueCmdData("AT+RSLV",RESP_STATUS_KEEP);	
		rt_thread_delay(1000);
		return WXC_ERROR;
	}
	else
	{		
		rt_kprintf("had connect\r\n");
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
/***************blue rev and send func******************/



void initBlue(char* devName)
{
	_set_Blue_At(PIN_LOW);
	if(at_client_init(devName, 512) == RT_EOK)
	{
		LOG_E("init success\r\n");
	}
}
int setBlueRevParm(int line_num,int delayTimeMs)
{
	arp = at_create_resp(512,line_num,rt_tick_from_millisecond(delayTimeMs));
	if(!arp)
	{
		LOG_E("No memory for response structure!");
        return -RT_ENOMEM;
	}	
	return RT_EOK;
}
int sendBlueCmdData(char *cmd,int resp_mode)
{		
	if (at_exec_cmd(arp, cmd) != RT_EOK)
	{
		LOG_E("AT client send commands failed, response error or timeout !");
		return -RT_ERROR;
	}
	if(resp_mode == RESP_STATUS_DELETE)
	{
		at_delete_resp(arp);	 
	}
	return RT_EOK;
}

int getRevDataForKey(char *key,char *revBuff)
{
	const char *rev;
	rev = at_resp_get_line_by_kw(arp,key);
	if(!rev)
	{
		LOG_E("AT client rev commands failed, response error or timeout !");
        return -RT_ERROR;
	}
	rt_memcpy(revBuff, rev, rt_strlen(rev));
	return RT_EOK;
}



int getRevDataForLine(int line,char *revBuff)
{
	const char *rev;
	rev = at_resp_get_line(arp,line);
	if(!rev)
	{
		LOG_E("AT client rev commands failed, response error or timeout !");
        return -RT_ERROR;
	}
	rt_memcpy(revBuff, rev, rt_strlen(rev));
	return RT_EOK;
}
void ansyBlueDatafunc(const char *data, rt_size_t size)
{
	LOG_D("AT Client receive ansy data!");
	for(int i = 0 ; i < size; i ++)
	{
		input_blueTooth(data[i]);
	}
}
void urc_recv_func(const char *data, rt_size_t size)
{
	 LOG_E("AT Client receive AT Server data!");
}


static struct at_urc urc_table[] = 
{
    {"+",            "R",          urc_recv_func},
  //  {{0xFF,0xFF},		  {0xAA,0xAA},     ansyBlueDatafunc},
};

int initBlueUrc(void)
{
	at_set_urc_table(urc_table,ARRAY_SIZE(urc_table));
	//at_set_end_sign('R');
	return RT_EOK;
}

int sendDataToBle(char *buff,int len)
{
	return at_client_send(buff,len);
}
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


