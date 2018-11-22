#ifndef __APP_
#define __APP_
#include <stm32f4xx_hal.h>



#define NUM1_Pin GPIO_PIN_8
#define NUM1_GPIO_Port GPIOE
#define NUM2_Pin GPIO_PIN_9
#define NUM2_GPIO_Port GPIOE
#define NUM3_Pin GPIO_PIN_10
#define NUM3_GPIO_Port GPIOE


#define KEY_1_Pin GPIO_PIN_0
#define KEY_1_GPIO_Port GPIOC
#define KEY_2_Pin GPIO_PIN_1
#define KEY_2_GPIO_Port GPIOC
#define KEY_3_Pin GPIO_PIN_2
#define KEY_3_GPIO_Port GPIOC
#define KEY_4_Pin GPIO_PIN_3
#define KEY_4_GPIO_Port GPIOC


extern void rt_app_application_init(void);
extern void rt_led_num_application_init(void);


extern void lightNumLed(int speed);
extern int getSetShowNum(int showNum);












extern rt_mutex_t ledMutex;












#endif
