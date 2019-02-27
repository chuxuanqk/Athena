#include "tim8.h"


void GPIOC_7_8_Init(void);
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void TIM8_PWM_Init(void)
{
    TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

    TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
}


void TIM8_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;    // TIM8 Update Interrupt 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}


void TIM8_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_Structure;

    TIM_Structure.TIM_Period = arr;                        //设置自动重装载值
    TIM_Structure.TIM_Prescaler = psc;                     //设置预分频值
    TIM_Structure.TIM_ClockDivision = 0;                   //设置时钟分割：TDTS = Tck_tim
    TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM向上计数
    
    TIM_TimeBaseInit(TIM8, &TIM_Structure);                //初始化TIM2
}


void TIM8_CH2_CH3_Init(uint16_t arr, uint16_t psc)
{
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);        // 使能TIM8
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);       // 使能GPIOC

    GPIOC_7_8_Init();
    TIM8_NVIC_Init();
    TIM8_Init(arr, psc);
    TIM8_PWM_Init();

    TIM_SetAutoreload(TIM2, arr);               // 设置自动重装载值
	TIM_SetCompare1(TIM2, arr/2);               // 设置比较值

    // TIM_Cmd(TIM8, ENABLE);                   //使能TIM8
}