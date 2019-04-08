#include "machControl.h"
#include <rtthread.h>
#include "dac.h"
#include "app.h"
#include "debugUsart.h"
#include "machOperation.h"

int g_nowSpeed = 0;

static rt_device_t mach_device;
stuMacnSendInfo g_stuMachInfo;

static ErrorStatus s_machStatus = SUCCESS;


/********************
@brief init value and info
@note 
@param 
@retval SUCCESS/ERROR
********************/
void machInit(rt_device_t mach_usart)
{
	if (mach_usart != RT_NULL)
	{
		mach_device = mach_usart;				
	}	
	g_stuMachOpt.open(&g_stuMachInfo);
}


/********************
@brief send buff for device 
@note 
@param 
@retval 
********************/
void machBuffWrite(char *buff,int len)
{
	if (mach_device != RT_NULL)
	{
		rt_device_write(mach_device, 0,buff,MACN_SEND_DATA_LEN);
	}
}
/********************
@brief rev buff for device 
@note 
@param 
@retval
********************/
void machBuffRead(char *buff,int *len)
{
	int rx_length = 0;
	if (mach_device != RT_NULL)
	{
		*len = rt_device_read(mach_device, 0, &(buff[0]),11);
		
	}
}

/********************
@brief rev buff for device 
@note 
@param 
@retval
********************/
void setMacnStatus(ErrorStatus sta)
{	
	s_machStatus = sta;
}


/********************
@brief return macn run status
@note void
@param void
@retval SUCCESS/ERROR
********************/

ErrorStatus checkMacnStatus(void)
{	
	return s_machStatus;
}
ErrorStatus ansyMacnData(uint8_t ch)
{	
	ErrorStatus rpy = SUCCESS;

	static uint8_t macnFlag = MACH_HEAD;
	static uint8_t macnStatus[4];
	static uint8_t addCrc[2];	
	static int revCount = 0;
	static int crc = 0;
	switch(macnFlag)
	{
		case MACH_SW_HEAD:
			if(ch == MACH_HEAD)
			{
				macnFlag = MACH_SW_ADDRESS;			
			}			
			break;
		case MACH_SW_ADDRESS:
			if(ch == MACH_ADDRESS)
			{
				macnFlag = MACH_SW_FUNCODE;			
			}
			else
			{
				macnFlag = MACH_SW_HEAD;
			}
			break;
		case MACH_SW_FUNCODE:
			if(ch == MACH_FUNCODE)
			{
				macnFlag = MACH_SW_LEN;	
				crc += ch;
			}
			else
			{
				macnFlag = MACH_SW_HEAD;
				crc = 0;
			}
			break;
		case MACH_SW_LEN:
			if(ch == MACH_REV_LEN)
			{
				macnFlag = MACH_SW_DATA;	
				crc += ch;
			}
			else
			{
				macnFlag = MACH_SW_HEAD;
				crc = 0;
			}
			break;
		case MACH_SW_DATA:	
			macnStatus[revCount++] = ch;
			crc += ch;
			if(revCount == MACH_REV_LEN)
			{
				macnFlag = MACH_SW_CRC;
				revCount = 0;
			}
			break;
		case MACH_SW_CRC:	
			addCrc[revCount++] = ch;			
			if(revCount == 2)
			{
				if(addCrc[0] == ((crc & 0xFF00) >> 8) && addCrc[1] == (crc &0x00FF))
				{
					macnFlag = MACH_SW_TITLE;
				
				}
				else
				{
					macnFlag = MACH_SW_HEAD;
				}
				revCount = 0;	
				crc = 0;
			}
			break;
		case MACH_SW_TITLE:
			if(ch == MACH_TITLE)
			{
				//rev data finsh and set status
				//todo:macnStatus
				if(macnStatus[0] > 0x01)
				{
					// error
					//ansyMacnStatus(macnStatus[1]);   //on the base of protocol 		
					s_machStatus = ERROR;
					rpy = ERROR;					
				}
				else
				{				
					// mach run ok
					g_nowSpeed = (int)((macnStatus[2] << 8) | macnStatus[3]);
					sendSignalDataToPc(macnStatus[2],macnStatus[3]);
				}				
			}
			macnFlag = MACH_SW_HEAD;
			break;
	}
	return rpy;
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

}


