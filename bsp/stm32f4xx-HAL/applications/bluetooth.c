#include "bluetooth.h"
#include <rtthread.h>
#include "math.h"
#include <drivers/pin.h>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "app.h"
extern rt_device_t Bluewrite_device;
void _ansyBlueStatus(uint8_t * blueData);
void _initBlueStatus(uint8_t * blueData);
void input_blueTooth(uint8_t ch)
{
	static uint8_t blueFlag = BLUE_HEAD;
	static uint8_t blueStatus[BLUE_LEN];
	//static uint8_t addCrc[2];
	static int revCount = 0;
	static int funCode = 0;
//	short crc = 0;
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
			//addCrc[revCount++] = ch;
			if(revCount == 2)
			{
				blueFlag = BLUE_TITLE;
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


static int s_initRevData[BLUE_LEN/2] = {0};
//what is this?
static int runA = 4;
static int runB = -20;

void getRunFuncData()
{
	runA = g_flashData.qMaxSpeed/(g_flashData.bodyAngle - g_flashData.stoprun);
	runB = -1 * g_flashData.stoprun * runA;
}
void _initBlueStatus(uint8_t * blueData)
{	
	for(int i = 0 ; i < BLUE_LEN/2 ; i++)
	{
		s_initRevData[i] = (blueData[i*2] << 8 | blueData[i*2+1]);
	}
	if(s_initRevData[0] > 0x8000)
	{
		s_initRevData[0] -= 65535;
	}	
	if(s_initRevData[1] > 0x8000)
	{
		s_initRevData[1] -= 65535;
	}
	if(s_initRevData[2] > 0x8000)
	{
		s_initRevData[2] -= 65535;
	}
	getRunFuncData();
	//rt_kprintf("init:%d,%d\r\n",s_initRevData[1],s_initRevData[0]);
}
int MacnWorkStatus;
//static int _MacnWorkStatus;

stuBodyExecInfo gBodyExecInfo;

int getRunData(int revData)
{
	int rpy = WXC_OK;
	short data = 0;
	MacnWorkStatus = 0;
	if(revData > 0x8000)
	{
		revData -= 65535;
	}
	float diffSpeed = revData - s_initRevData[1];			//jiaodu 
	if(diffSpeed >= g_flashData.stoprun)
	{
		data = (short)(runA*diffSpeed +runB);
		MacnWorkStatus |=BLUE_FORWARD ;
		data = data>g_flashData.qMaxSpeed?g_flashData.qMaxSpeed:data;
		data = data<=0?0:data;
	}
	else if(diffSpeed < g_flashData.stoptui)
	{		
		if(diffSpeed < 0)
			diffSpeed *= -1;
		data = (short)(3*diffSpeed);	
		MacnWorkStatus |=BLUE_BACKUP;
		data = data>g_flashData.hMaxSpeed?g_flashData.hMaxSpeed:data;
		data = data<=0?0:data;		
	}
	else
	{
		data = 0;
		MacnWorkStatus = BLUE_STOP;
	}
	data = getClacData(data);
	MacnWorkStatus |= data;
	gBodyExecInfo.run = MacnWorkStatus;
	//rt_kprintf("run:%x\r\n",gBodyExecInfo.run);
	return rpy;
}

int getRightData(int revData)
{
	int rpy = WXC_OK;
	short data = 0;
	MacnWorkStatus = 0;
	if(revData > 0x8000)
	{
		revData -= 65535;
	}
	float diffSpeed = revData - s_initRevData[2];
	if(diffSpeed >= 5)
	{
		//zuo you zhuan	
		data = (short)(4*diffSpeed - 20);
		MacnWorkStatus |= BLUE_RIGHT;	
	}
	else if(diffSpeed < -5)
	{
		diffSpeed *= -1;
		data = (short)(4*diffSpeed - 20);
		MacnWorkStatus |= BLUE_LEFT;
		
	}
	else
	{
		data = 0;
		MacnWorkStatus |= BLUE_RIGHT;
	}
	MacnWorkStatus |= data ;
	gBodyExecInfo.right = MacnWorkStatus;
	rt_kprintf("right:%x\r\n",gBodyExecInfo.right);
	return rpy;
}

int getCeBodyData(int revData)
{
	int rpy = WXC_OK;
	short data = 0;
	MacnWorkStatus = 0;
	if(revData > 0x8000)
	{
		revData -= 65535;
	}
	float diffSpeed = revData - s_initRevData[2];
	if(diffSpeed >= 5)
	{
		//zuo you zhuan	
		data = (short)(4*diffSpeed - 20);
		MacnWorkStatus |= BLUE_CELEFT;	
	}
	else if(diffSpeed < -5)
	{
		diffSpeed *= -1;
		data = (short)(4*diffSpeed - 20);
		MacnWorkStatus |= BLUE_CERIGHT;
		
	}
	else
	{
		data = 0;
		MacnWorkStatus |= BLUE_CERIGHT;
	}
	MacnWorkStatus |= data ;
	gBodyExecInfo.ceBody = MacnWorkStatus;
	rt_kprintf("ceBody:%x\r\n",gBodyExecInfo.ceBody);
	return rpy;
}

void _ansyBlueStatus(uint8_t * blueData)
{	
	MacnWorkStatus = 0;
	int revData[BLUE_LEN/2] = {0};
	for(int i = 0 ; i < BLUE_LEN/2 ; i++)
	{
		revData[i] = (blueData[i*2] << 8 | blueData[i*2+1]);
	}
	
//clac macn work status
#if DEVICE_CE_BODY_DATA == HAL_OK
	getCeBodyData(revData[0]);	
#endif	
#if DEVICE_RUN_BODY_DATA == HAL_OK	
	getRunData(revData[1]);
#endif	
#if DEVICE_RIGHT_BODY_DATA == HAL_OK		
	getRightData(revData[2]);
#endif	
	return;

}

short getClacData(short tData)
{
	static short s_data[CALC_DATA_COUNT];
	static int s_index = 0;
	static double count = 0;
	short temp;
	count ++;
	s_data[s_index++] = tData;
	s_index = s_index%CALC_DATA_COUNT;
	if(count < CALC_DATA_COUNT)
	{
		return tData;
	}
	double sum = 0;
	for(int i = 0 ; i < CALC_DATA_COUNT ; i ++)
	{
		sum += s_data[i];
	}
	temp = (short)(sum/CALC_DATA_COUNT);
	if(tData > temp*1.2)
	{
		temp = temp*1.2;
	}
	else if(tData < temp*0.8)
	{
		temp = temp*0.8;
	}
	else
	{
		temp = tData;
	}
	return temp;
}
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
	rt_device_write(Bluewrite_device,0,buff,strlen((char *)buff));
	//rt_kprintf("%s",buff);
}
void _sendCmdToBlue(uint8_t *buff,int len)
{	
	for(int i = 0 ; i < len ; i ++)
	{
		rt_kprintf(" %02x ",buff[i]);
	}
	rt_device_write(Bluewrite_device,0,buff,len);
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
int initBlueSet()
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

static void _ansyBlueCmdData(char * revbuff,int len)
{
	rt_kprintf("blue rev :%S,%d\r\n",revbuff,len);
}
void input_blueTooth_cmd(char ch)
{
	static char s_buff[64] = {0};
	static int s_index=0;
	s_buff[s_index++] = ch;
	if(ch == 0xAA)
	{
		//had end rev data
		_ansyBlueCmdData(s_buff,s_index);
		s_index = 0;
	}
}


