#ifndef __DEBUG__
#define __DEBUG_
#include <stm32f4xx_hal.h>

#define REV_BUFF_COUNT	100
#define REV_DEVICE_ID	0x01
#define REV_DATA_MODE	0x03
#define REV_CMD_MODE	0x06

#define REV_CMD_INDEX_HIGH	0x02
#define REV_CMD_INDEX_LOW	0x03

#define REV_DATA_INDEX_HIGH	0x04
#define REV_DATA_INDEX_LOW	0x05

#define SEND_DEVICE_ID  0x01
#define SEND_DATA_MODE  0x03


//rev cmd tyep

#define REV_CMD_TYPE_RUN_MODE 			0x01
#define REV_CMD_TYPE_STOP_DATA 			0x02
#define REV_CMD_TYPE_RUN_SPEED 			0x03
#define REV_CMD_TYPE_HOUTUI_INFO 		0x04
#define REV_CMD_TYPE_RIGHT_INFO 		0x05
#define REV_CMD_TYPE_ZHUAN_INFO 		0x06
#define REV_CMD_TYPE_GET_INFO 			0x07









typedef struct debuginfo
{	
	char name[10];
}stu_debufInfo;

extern void saveFalshData(void);
extern void debugAnsy(char ch);
extern void sendSignalDataToPc(char data1,char data2);
#define ARRARCOUNT(__X__)    (sizeof(__X__)/sizeof(__X__[0]))









#endif

