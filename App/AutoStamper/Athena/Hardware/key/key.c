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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道

    NVIC_Init(&NVIC_InitStructure);
}


void NVIC3_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道

    NVIC_Init(&NVIC_InitStructure);
}


void NVIC4_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;   //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;          //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能外部中断通道

    NVIC_Init(&NVIC_InitStructure);
}


void EXTIX_Init(void)
{
	KEY_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIOE.4中断线以及中断初始化配置，下降沿触发
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);

    // 外部中断初始化
    EXTI2_Init();
    EXTI3_Init();
    EXTI4_Init();
    
    //配置NVICx
    NVIC2_Init();
    NVIC3_Init();
    NVIC4_Init();

}



//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
    delay_ms(10);       //消抖

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


//外部中断3服务程序
void EXTI3_IRQHandler(void)
{	
	delay_ms(10);       //消抖
	if(S2 == 0)
	{
		//LED = !LED;
		STEP = STEP-(4*800);
		FLAG = 1;
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}


//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
	delay_ms(10);       //消抖
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

