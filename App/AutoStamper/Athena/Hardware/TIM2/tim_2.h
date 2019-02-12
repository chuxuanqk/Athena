#ifndef __TIM_2_H
#define __TIM_2_H

#include "stm32f10x.h"

//≤ø∑÷÷ÿ”≥…‰2
void GPIOA_0_CH1_Init(void);
void GPIOA_1_CH2_Init(void);
void GPIOB_10_CH3_Init(void);
void GPIOB_11_CH4_Init(void);

void TIM2_PWM_Init(void);
void TIM2_NVIC_Init(void);
void TIM2_RemapConfig_Init(void);
void TIM2_Init(uint16_t arr,uint16_t psc);

void TIM2_CH1_Init(uint16_t arr, uint16_t psc);
void TIM2_CH3_CH4_Init(uint16_t arr,uint16_t psc);

//void TIM2_CH3_PWM_Init(void);
//void TIM2_CH4_PWM_Init(void);

#endif
