#ifndef HAL_PWM
#define HAL_PWM
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"


extern void 		MX_TIM3_Init(void);

extern void 		user_pwm_setvalue(uint16_t value,uint32_t pwmIndex);
extern void 		HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);
	

#define LSTICK_H	1

#define LSTICK_V	2

#define RSTICK_H	3

#define RSTICK_V	4






#ifdef __cplusplus
}
#endif




#endif


