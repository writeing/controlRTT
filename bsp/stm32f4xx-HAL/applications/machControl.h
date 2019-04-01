#ifndef __MACHCONTROL__
#define __MACHCONTROL__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "bluetooth.h"	 
/*********************************define********************************/	 
#define NONE			0
#define MACH_HEAD		0x55	 
#define MACH_ADDRESS 	0x01	 
#define MACH_FUNCODE	0x02
#define MACH_LEN		0x04
#define MACH_DATA		0xA5
#define MACH_CRC		0x06
#define MACH_TITLE		0xAA
#define MACH_SEND_FUNCODE	0x81
	 
#define MACN_FORWARD     0x01
#define MACN_BACKUP      0x02
#define MACN_LEFT        0x04
#define MACN_RIGHT       0x08
#define MACN_STOP        0x00	 
#define MACN_CERIGHT     0x10
#define MACN_CELEFT      0x11	 

	 
#define MACN_BEGIN		0x01
#define MACN_END		0x02
	 
	 
#define MACN_SEND_DATA_LEN      9
/*********************************value********************************/

typedef struct Machcontrol
{
	char HEAD;
	char address;
	char FuncCode;
	char dataLen;
	char sbuff[2];
	short crc;
	char title;
	char sendbuff[9];
	char FLag;
}stuMacnControl;		


/*********************************func********************************/
extern void setSendData(char data1,char data2);
extern char *getSendData(void);
extern void checkMacnData(unsigned char ch);
extern void setMacbWorkStatus(stuBodyExecInfo status);
extern int getMacnRunStatus(void);
extern void initMacninfo(int FuncCode);
extern void checkMachStatus(rt_device_t machUsart);
#ifdef __cplusplus
}
#endif




#endif

