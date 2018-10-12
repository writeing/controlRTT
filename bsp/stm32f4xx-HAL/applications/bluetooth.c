#include "bluetooth.h"
#include <rtthread.h>
#include "math.h"
void _ansyBlueStatus(uint8_t * blueData);
void _initBlueStatus(uint8_t * blueData);
void input_blueTooth(uint8_t ch)
{
	static uint8_t blueFlag = BLUE_HEAD;
	static uint8_t blueStatus[BLUE_LEN];
	static int revCount = 0;
	static int funCode = 0;
	uint8_t crc = 0;
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
			break;
		case BLUE_LEN:
			if(ch == BLUE_LEN)
				blueFlag = BLUE_DATA;	
			else
				blueFlag = BLUE_HEAD;
			break;
		case BLUE_DATA:			
			if(revCount == BLUE_LEN)
			{
				blueFlag = BLUE_CRC;
				revCount = 0;
			}
			else
			{
				blueStatus[revCount++] = ch;				
			}
			break;
		case BLUE_CRC:			
			for(int i = 0 ; i < BLUE_LEN ; i ++)
			{
				crc += blueStatus[i];
			}
			if(ch == ((uint8_t)(BLUE_FUNCODE + BLUE_LEN + crc)))
			{
				blueFlag = BLUE_TITLE;
			}
			else
			{
				blueFlag = BLUE_HEAD;
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


static float s_initRevData[BLUE_LEN/2] = {0};
void _initBlueStatus(uint8_t * blueData)
{	
	for(int i = 0 ; i < BLUE_LEN/2 ; i++)
	{
		s_initRevData[i] = (blueData[i*2] << 8 | blueData[i*2+1])/100;
	}
}
static uint8_t MacnWorkStatus;
void _ansyBlueStatus(uint8_t * blueData)
{
	float revData[BLUE_LEN/2] = {0};
	for(int i = 0 ; i < BLUE_LEN/2 ; i++)
	{
		revData[i] = (blueData[i*2] << 8 | blueData[i*2+1])/100;
	}
	//clac macn work status	
	float diffSpeed = 0;
	if(revData[1] < 0)
	{
		diffSpeed = revData[1] + 180 + s_initRevData[1];
		MacnWorkStatus |= BLUE_FORWARD;
	}
	else
	{
		diffSpeed = revData[1] - s_initRevData[1];
		MacnWorkStatus |= BLUE_BACKUP;
	}
	//You layer the velocity
	MacnWorkStatus |= (char)(0.147*pow(diffSpeed,1.1271));
}


uint8_t getBlueMacnStatus()
{
	return MacnWorkStatus;
}

void initBlueSet()
{
	
}





