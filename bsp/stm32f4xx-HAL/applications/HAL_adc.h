#ifndef HAL_ADC
#define HAL_ADC

#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx_hal.h"



extern void 		MX_ADC1_Init(void);

extern void 		AdcGetValue(uint32_t *channel1,uint32_t *channel2);






#ifdef __cplusplus
}
#endif























#endif

