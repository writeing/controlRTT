#ifndef __BLUETOOTH__
#define __BLUETOOTH__

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


#define BLUE_FORWARD     0x10000
#define BLUE_BACKUP      0x20000
#define BLUE_LEFT        0x40000
#define BLUE_RIGHT       0x80000
#define BLUE_STOP	     0x00000

#define WXC_OK       1
#define WXC_ERROR    0






extern void input_blueTooth(unsigned char ch);
extern int getBlueMacnStatus(void);
extern int initBlueSet(void);
extern void input_blueTooth_cmd(char ch);
extern void blueCmdSend(int cmd);








#endif

