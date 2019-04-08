#include "ansyBlueData.h"
#include "bluetooth.h"


void _ansyBlueStatus(uint8_t * blueData);
void _initBlueStatus(uint8_t * blueData);

void input_blueTooth(uint8_t ch)
{
	static uint8_t blueFlag = BLUE_HEAD;
	static uint8_t blueStatus[BLUE_LEN];
	//static uint8_t addCrc[2];
	static int revCount = 0;
	static int funCode = 0;
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

























