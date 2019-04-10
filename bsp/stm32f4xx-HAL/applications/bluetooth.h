#ifndef __BLUETOOTH__
#define __BLUETOOTH__
#include "app.h"
#include <board.h>




#define ARRAY_SIZE(ar) (sizeof(ar) / sizeof(ar[0]))



#define NONE			0
#define BLUE_HEAD		0x55
#define BLUE_FUNCODE	0x03
#define BLUE_LEN		0x06
#define BLUE_DATA		0xA5
#define BLUE_CRC		0x04
#define BLUE_TITLE		0xCC

#define BLUE_INITFUNCODE	0x06
#define BLUE_STARTCODE		0x02
#define BLUE_STOPCODE		0x01


#define BLUE_FORWARD     0x010000
#define BLUE_BACKUP      0x020000
#define BLUE_LEFT        0x040000
#define BLUE_RIGHT       0x080000
#define BLUE_CERIGHT     0x100000
#define BLUE_CELEFT      0x110000


#define BLUE_STOP	     0x00000

#define WXC_OK       1
#define WXC_ERROR    0

#define CALC_DATA_COUNT		50

typedef struct bodyexecInfo
{
	int run;
	int right;
	int ceBody;
}stuBodyExecInfo;


//extern void 			input_blueTooth(unsigned char ch);
extern stuBodyExecInfo 	getBlueMacnStatus(void);
//extern int 				initBlueSet(void);
//extern void 			input_blueTooth_cmd(char ch);
//extern void 			blueCmdSend(int cmd);
extern short 			getClacData(short tData);
//extern int 				getBlueConnectStatus(void);

//new

#define RESP_STATUS_DELETE		1			
#define RESP_STATUS_KEEP		0


extern void 			initBlue(char* devName);
extern int 				initBlueUrc(void);

extern int 				setBlueRevParm(int line_num,int delayTimeMs);

extern int 				sendBlueCmdData(char *cmd,int resp_mode);
extern int 				getRevDataForKey(char *key,char *revBuff);
extern int 				getRevDataForLine(int line,char *revBuff);

extern int 				sendDataToBle(char *buff,int len);


extern int 				initBlueSet(void);

extern stuFlashData g_flashData;
extern stuBodyExecInfo gBodyExecInfo;





#endif

