#ifndef MACHOPERATION
#define MACHOPERATION



/**************begin define parm***********************/

#define NONE					0
#define MACH_HEAD				0x55	 
#define MACH_ADDRESS 			0x01	 
#define MACH_FUNCODE			0x02
#define MACH_LEN				0x02
#define MACH_DATA				0xA5
#define MACH_CRC				0x06
#define MACH_TITLE				0xAA
#define MACN_SEND_DATA_LEN      9
#define MACH_REV_LEN			0x04


typedef struct Machcontrol
{
	char HEAD;
	char address;
	char FuncCode;
	char dataLen;
	char sbuff[2];
	short crc;
	char title;
}stuMacnSendInfo;		



typedef struct file_operation
{
	ErrorStatus (*open)(stuMacnSendInfo *stumach);
	ErrorStatus (*write)(stuMacnSendInfo *stumach,char direct,int speed);
	ErrorStatus (*read)(char *revBuff,int *revlen);
	ErrorStatus (*ioctl)(void);
}stuMachOperation;		




/*************end define parm********************/




/**************begin define func***********************/
extern void __weak	machBuffWrite(char *buff,int len);
extern void __weak	machBuffRead(char *buff,int *len);

extern ErrorStatus 	initMach(stuMacnSendInfo *stumach);
extern ErrorStatus 	machSendBuffToDev(stuMacnSendInfo *stumach,char direct,int speed);
extern ErrorStatus 	MachRevBuffForDev(char *revBuff,int *revlen);
extern ErrorStatus 	MachIoctl(void);






/*************end define func********************/


/**************begin define value***********************/


extern stuMachOperation g_stuMachOpt;






/*************end define value********************/






#endif

