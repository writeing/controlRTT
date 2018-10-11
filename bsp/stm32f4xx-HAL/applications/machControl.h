#ifndef __MACHCONTROL__
#define __MACHCONTROL__

#ifdef __cplusplus
 extern "C" {
#endif
	 	
#define NONE			0
#define MACH_HEAD		0x55
#define MACH_FUNCODE	0x03
#define MACH_LEN		0x02
#define MACH_DATA		0xA5
#define MACH_CRC		0x04
#define MACH_TITLE		0xAA

	 
typedef struct Machcontrol
{
	char HEAD;
	char FuncCode;
	char dataLen;
	char sbuff[2];
	char crc;
	//char crcL;
	char title;
	char sendbuff[7];
}stuMacnControl;		












#ifdef __cplusplus
}
#endif




#endif

