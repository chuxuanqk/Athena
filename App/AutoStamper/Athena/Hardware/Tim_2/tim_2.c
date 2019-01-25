#include "tim_2.h"


// ������ӳ��2
void GPIOA_0_CH1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void GPIOA_1_CH2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void GPIOB_10_CH3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void GPIOB_11_CH4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


//��ʼ��TIM2
void TIM2_Init(uint32_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM2_Structure;

    TIM2_Structure.TIM_Period = arr;       //�����Զ���װ��ֵ
    TIM2_Structure.TIM_Prescaler = psc;    //����Ԥ��Ƶֵ
    TIM2_Structure.TIM_ClockDivision = 0;  //����ʱ�ӷָTDTS = Tck_tim
    TIM2_Structure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM���ϼ���
    
    TIM_TimeBaseInit(TIM2, &TIM2_Structure);     //��ʼ��TIM2
}


TIM_OCInitTypeDef TIM2_PWM_Init(void)
{
	TIM_OCInitTypeDef TIM2_OCInitStructure;
	
	TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	return TIM2_OCInitStructure;
}


void TIM2_RemapConfig_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);         //ʹ�ܶ�ʱ��TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);
}


/************************************************************
 *                      ��ʼ��ֱ���������
 ************************************************************/
void TIM2_CH3_CH4_Init(uint32_t arr,uint16_t psc)
{
	TIM_OCInitTypeDef TIM2_OCInitStructure;

    TIM2_RemapConfig_Init()
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIOB_10_CH3_Init();
    GPIOB_11_CH4_Init();
    TIM2_Init(arr,psc);
	
	// ����TIM2��PWMģʽ
	TIM2_OCInitStructure = TIM2_PWM_Init();
    // ��ʼ��CH3��CH4
    TIM_OC3Init(TIM2, &TIM2_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM2_OCInitStructure);

    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
//    TIM_SetAutoreload(TIM2, arr);
	
    TIM_Cmd(TIM2, ENABLE);
}


/************************************************************
 *                      ��ʼ��X�Ჽ�����
 ************************************************************/
void TIM2_CH1_Init(uint32_t arr, uint16_t psc)
{
    TIM_OCInitTypeDef TIM2_OCInitStructure;

    TIM2_RemapConfig_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIOA_0_CH1_Init();
    TIM2_Init(arr, psc);

    //����TIM2��PWMģʽ
    TIM2_OCInitStructure = TIM2_PWM_Init();
    // ��ʼ��CH1
    TIM_OC1Init(TIM2, &TIM2_OCInitStructure);
    
	TIM_SetCompare1(TIM2, arr/2);
    TIM_SetAutoreload(TIM2, arr);

    TIM_Cmd(TIM2, ENABLE);
}