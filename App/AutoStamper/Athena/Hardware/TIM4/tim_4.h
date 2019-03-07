#ifndef __TIM_4_H
#define __TIM_4_H

#include "stm32f10x.h"

void GPIOB_7_CH2_Init(void);

void TIM4_PWM_Init(void);
void TIM4_NVIC_Init(void);
void TIM4_Init(uint16_t arr, uint16_t psc);


#endif
