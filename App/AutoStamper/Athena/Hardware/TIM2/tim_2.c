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

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
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


void TIM2_PWM_Init(void)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	//TIM_OC1Init(TIM2, &TIM_OCInitStructure);   // ��ʼ��TIM2��CH1ΪPWN���
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	//TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	//TIM_OC4Init(TIM2, &TIM_OCInitStructure);
}


// ��ʼ��TIM2���ж�����
void TIM2_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}


// ��ӳ������
void TIM2_RemapConfig_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);        // ��������ʱ��
	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);       // ʹ��TIM2������ӳ��2
}


//��ʼ��TIM2
void TIM2_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_Structure;

    TIM_Structure.TIM_Period = arr;                        //�����Զ���װ��ֵ
    TIM_Structure.TIM_Prescaler = psc;                     //����Ԥ��Ƶֵ
    TIM_Structure.TIM_ClockDivision = 0;                   //����ʱ�ӷָTDTS = Tck_tim
    TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM���ϼ���
    
    TIM_TimeBaseInit(TIM2, &TIM_Structure);                //��ʼ��TIM2
}


//                      ���Դ���
/************************************************************
 *                      ��ʼ��X�Ჽ�����
 ************************************************************/
void TIM2_CH1_Init(uint16_t arr, uint16_t psc)
{
    //TIM_OCInitTypeDef TIM2_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);        // ʹ��TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // ʹ��GPIOA

    TIM2_RemapConfig_Init();                    // ������ӳ��
    GPIOA_0_CH1_Init();                         // ��ʼ��GPIOA_0
    TIM2_NVIC_Init();                           // ��ʼ��TIM2�ж�����
    TIM2_Init(arr, psc);                        // ��ʼ��TIM2
    TIM2_PWM_Init();                            // ����TIM2��PWMģʽ
    
    TIM_SetAutoreload(TIM2, arr);               // �����Զ���װ��ֵ
	TIM_SetCompare1(TIM2, arr/2);               // ���ñȽ�ֵ

    TIM_Cmd(TIM2, ENABLE);                      // ʹ��TIM2
}


/************************************************************
 *                      ��ʼ��ֱ���������
 ************************************************************/
void TIM2_CH3_CH4_Init(uint16_t arr,uint16_t psc)
{
	TIM_OCInitTypeDef TIM2_OCInitStructure;

    TIM2_RemapConfig_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIOB_10_CH3_Init();
    GPIOB_11_CH4_Init();
    TIM2_NVIC_Init();
    TIM2_NVIC_Init();
    TIM2_Init(arr,psc);
	
	// ����TIM2��PWMģʽ
	TIM2_PWM_Init();
    // ��ʼ��CH3��CH4
    TIM_OC3Init(TIM2, &TIM2_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM2_OCInitStructure);

    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
//    TIM_SetAutoreload(TIM2, arr);
	
    TIM_Cmd(TIM2, ENABLE);
}


