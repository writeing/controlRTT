#ifndef __BLUETOOTH__
#define __BLUETOOTH__

#define NONE			0
#define BLUE_HEAD		0x55
#define BLUE_FUNCODE	0x03
#define BLUE_LEN		0x06
#define BLUE_DATA		0xA5
#define BLUE_CRC		0x04
#define BLUE_TITLE		0xAA

#define BLUE_INITFUNCODE	0x06

#define BLUE_FORWARD     0x10
#define BLUE_BACKUP      0x20
#define BLUE_LEFT        0x30
#define BLUE_RIGHT       0x40








extern void input_blueTooth(unsigned char ch);
extern unsigned char getBlueMacnStatus(void);
extern void initBlueSet(void);










#endif

