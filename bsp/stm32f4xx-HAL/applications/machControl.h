#ifndef __MACHCONTROL__
#define __MACHCONTROL__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "bluetooth.h"	 
/*********************************define********************************/	 

#define MACH_SEND_FUNCODE	0x81

#define MACH_SW_HEAD				0x01	 
#define MACH_SW_ADDRESS 			0x02	 
#define MACH_SW_FUNCODE				0x03
#define MACH_SW_LEN					0x04
#define MACH_SW_DATA				0x05
#define MACH_SW_CRC					0x06
#define MACH_SW_TITLE				0x07



#define MACN_FORWARD     0x01
#define MACN_BACKUP      0x02
#define MACN_LEFT        0x04
#define MACN_RIGHT       0x08
#define MACN_STOP        0x00	 
#define MACN_CERIGHT     0x10
#define MACN_CELEFT      0x11	 

	 
#define MACN_BEGIN		0x01
#define MACN_END		0x02
	 
	 

/*********************************value********************************/



/*********************************func********************************/

extern ErrorStatus 	ansyMacnData(uint8_t ch);

extern void 		machInit(rt_device_t mach_usart);

extern ErrorStatus 	checkMacnStatus(void);

extern void 		setMacnStatus(ErrorStatus sta);


#ifdef __cplusplus
}
#endif




#endif

