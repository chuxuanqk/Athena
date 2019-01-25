#ifndef __TIM_2_H
#define __TIM_2_H

#include "stm32f10x.h"

//≤ø∑÷÷ÿ”≥…‰2
void GPIO_0_CH1_Init(void);
void GPIO_1_CH2_Init(void);
void GPIOB_10_CH3_Init(void);
void GPIOB_11_CH4_Init(void);

TIM_OCInitTypeDef TIM2_PWM_Init(void);
void TIM2_RemapConfig_Init();
void TIM2_Init(uint32_t arr,uint16_t psc);

void TIM2_CH3_CH4_Init(uint32_t arr,uint16_t psc);
void TIM2_CH1_Init(uint32_t arr, uint16_t psc);

//void TIM2_CH3_PWM_Init(void);
//void TIM2_CH4_PWM_Init(void);

#endif
