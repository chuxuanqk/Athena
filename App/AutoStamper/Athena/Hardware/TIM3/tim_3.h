#ifndef __TIM_3_H
#define __TIM_3_H

#include "stm32f10x.h"

void GPIOA_7_CH2_Init(void);

void TIM3_PWM_Init(void);
void TIM3_NVIC_Init(void);
void TIM3_Init(uint16_t arr, uint16_t psc);


#endif