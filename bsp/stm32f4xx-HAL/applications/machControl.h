#ifndef __MACHCONTROL__
#define __MACHCONTROL__

#ifdef __cplusplus
 extern "C" {
#endif
	 	 
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

