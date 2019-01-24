#include "tim_2.h"


void GPIOB_10_CH3_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void GPIOB_11_CH4_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


//初始化TIM2
void TIM2_Init(uint32_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM2_Structure;

    TIM2_Structure.TIM_Period = arr;       //设置自动重装载值
    TIM2_Structure.TIM_Prescaler = psc;    //设置预分频值
    TIM2_Structure.TIM_ClockDivision = 0;  //设置时钟分割：TDTS = Tck_tim
    TIM2_Structure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数
    
    TIM_TimeBaseInit(TIM2, &TIM2_Structure);     //初始化TIM2
}


void TIM2_PWM_Init(uint32_t arr,uint16_t psc)
{

    TIM_OCInitTypeDef TIM2_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);         //使能定时器TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	
	GPIOB_10_CH3_Init();
    GPIOB_11_CH4_Init();
    TIM2_Init(arr,psc);
	
    TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC3Init(TIM2, &TIM2_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM2_OCInitStructure);

    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
//    TIM_SetAutoreload(TIM2, arr);
	
    TIM_Cmd(TIM2, ENABLE);
}
