#include "tim_3.h"


void GPIOA_7_CH2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void TIM3_PWM_Init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);              
}


void TIM3_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}


void TIM3_Init(uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_Structure;

    TIM_Structure.TIM_Period = arr;                        
    TIM_Structure.TIM_Prescaler = psc;                     
    TIM_Structure.TIM_ClockDivision = 0;                   
    TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;   
    
    TIM_TimeBaseInit(TIM3, &TIM_Structure);                
}


