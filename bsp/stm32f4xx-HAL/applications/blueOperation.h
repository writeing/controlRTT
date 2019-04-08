#ifndef BLUEOPERATITON
#define BLUEOPERATITON

/**************begin define parm***********************/


typedef struct file_operation
{
	ErrorStatus (*open)(void);
	ErrorStatus (*write)(char *buff,int len);
	ErrorStatus (*read)(char *revBuff,int *revlen);
	ErrorStatus (*ioctl)(void);
}stublueOperation;		




/*************end define parm********************/




/**************begin define func***********************/
extern void __weak	blueBuffWrite(char *buff,int len);
extern void __weak	blueBuffRead(char *buff,int *len);

extern ErrorStatus 	initblue(void);
extern ErrorStatus 	blueSendBuffToDev(char *buff,int len);
extern ErrorStatus 	blueRevBuffForDev(char *revBuff,int *revlen);
extern ErrorStatus 	blueIoctl(void);






/*************end define func********************/


/**************begin define value***********************/
extern stublueOperation g_stublueOpt;









/*************end define value********************/






























#endif

