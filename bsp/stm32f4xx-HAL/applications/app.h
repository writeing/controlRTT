#ifndef __APP_
#define __APP_
#include <stm32f4xx_hal.h>


//**********begin sys define 
#define BUZZ     	95

#define RADREV	 	96

#define URG_LOCK 	5

#define BLE_PWRC 	52

#define BLE_AT 	 	51

#define BLE_CON	 	53

#define DEVICE_KEY 	39

#define KEY_1	 	0

#define KEY_2	 	1

#define KEY_3    	2


#define ADC_ML		23

#define ADC_MR		24

#define PWM_DAC1	63

#define PWM_DAC2	64

#define PWM_DAC3	65

#define PWM_DAC4	66




//**********end sys define 

//************begin user define CONFIG
#define DEVICE_NONE      	0
#define DEVICE_BEGIN     	1
#define DEVICE_END       	2

#define SHOW_SPEED_MODE   	1

#define SHOW_XXXX_MODE    	2

#define SHOW_YYYY_MODE    	3


//define debug config
#define DEVICE_RUN_BODY_DATA				HAL_OK
#define DEVICE_CE_BODY_DATA					HAL_OK
#define DEVICE_RIGHT_BODY_DATA				HAL_OK
#define DEVICE_MACH_PRINT					HAL_OK	
#define DEVICE_BLUE_PRINT_DATA_INTERVAL		10

//***********end user define CONFIG

//***********begin user define func
extern void rt_app_application_init(void);
extern void rt_led_num_application_init(void);
extern void lightNumLed(int data,int index,int mode);
extern int getSetShowNum(int showNum);
extern void initExternFlash(void);		//in main thread   init extern flash

extern void rt_MacnApp_application_init(void);
extern void rt_BlueApp_application_init(void);
extern void getBlueCmdData(void);
//***********end user define func

//***********begin user define value
typedef struct flashData
{
	char blueDevice[20];
	char bodyAngle;
	char qMaxSpeed;
	char hMaxSpeed;	
	char runmode;
	char stoptui;
	char stoprun;	
	char houJiaoDu;
}stuFlashData;


struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};


//***********end user define value

extern rt_mutex_t ledMutex;
extern  uint8_t deviceStatus;
extern int blue_data_type;







#endif
