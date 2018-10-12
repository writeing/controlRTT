#ifndef __MACHCONTROL__
#define __MACHCONTROL__

#ifdef __cplusplus
 extern "C" {
#endif
	 	
/*********************************define********************************/	 
#define NONE			0
#define MACH_HEAD		0x55
#define MACH_FUNCODE	0x03
#define MACH_LEN		0x02
#define MACH_DATA		0xA5
#define MACH_CRC		0x04
#define MACH_TITLE		0xAA

	 
#define MACN_FORWARD     0x10
#define MACN_BACKUP      0x20
#define MACN_LEFT        0x30
#define MACN_RIGHT       0x40
	 
	 
#define MACN_BEGIN		0x01
#define MACN_END		0x02
/*********************************value********************************/

typedef struct Machcontrol
{
	char HEAD;
	char FuncCode;
	char dataLen;
	char sbuff[2];
	char crc;
	char title;
	char sendbuff[7];
	char FLag;
}stuMacnControl;		


/*********************************func********************************/
extern void setSendData(char data1,char data2);
extern char *getSendData(void);
extern void checkMacnData(char ch);
extern void setMacbWorkStatus(unsigned char status);
extern int getMacnRunStatus(void);
extern void initMacninfo(int FuncCode);
#ifdef __cplusplus
}
#endif




#endif

