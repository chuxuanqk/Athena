#ifndef __TIM_8_H
#define __TIM_8_H

#include "stm32f10x.h"

// 机械抓手PWM控制L298N驱动器
void GPIOC_7_8_Init(void);

void TIM8_PWM_Init(void);
void TIM8_NVIC_Init(void);
void TIM8_Init(void);
void TIM8_CH2_CH3_Init(uint16_t arr, uint16_t psc);


#endif