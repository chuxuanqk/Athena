#include "key.h"
#include "led.h"
#include "delay.h"


uint32_t STEP=0;
uint8_t FLAG = 0;

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void EXTI2_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}


void EXTI3_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}


void EXTI4_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);
}


void NVIC2_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   //��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��

    NVIC_Init(&NVIC_InitStructure);
}


void NVIC3_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   //��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��

    NVIC_Init(&NVIC_InitStructure);
}


void NVIC4_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   //��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ���ⲿ�ж�ͨ��

    NVIC_Init(&NVIC_InitStructure);
}


void EXTIX_Init(void)
{
	KEY_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIOE.4�ж����Լ��жϳ�ʼ�����ã��½��ش���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);

    // �ⲿ�жϳ�ʼ��
    EXTI2_Init();
    EXTI3_Init();
    EXTI4_Init();
    
    //����NVICx
    NVIC2_Init();
    NVIC3_Init();
    NVIC4_Init();

}



//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
    delay_ms(10);       //����

    if(S3 == 0)
    {
        //GPIO_SetBits(GPIOB, GPIO_Pin_7);
		//LED = !LED;
		STEP = 0;
		FLAG = 1;
		//stop();
    }

    EXTI_ClearITPendingBit(EXTI_Line2);
}


//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{	
	delay_ms(10);       //����
	if(S2 == 0)
	{
		//LED = !LED;
		STEP = STEP-(4*800);
		FLAG = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}


//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{
	delay_ms(10);       //����
	if(S1 == 0)
	{
		STEP = STEP+(4*800);
		FLAG = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}


void stop(void)
{
	TIM_SetCompare3(TIM2, 0);
	TIM_SetCompare4(TIM2, 0);
}

