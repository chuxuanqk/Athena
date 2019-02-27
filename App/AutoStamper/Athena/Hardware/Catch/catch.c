#include "catch.h"


void Catch_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&GPIO_InitStructure);	
	
    GPIO_SetBits(GPIOC,GPIO_Pin_7);
    GPIO_SetBits(GPIOC,GPIO_Pin_8);
}


void Catch(sw SW)
{
	switch(SW)
	{
		case OPEN:
			GPIO_SetBits(GPIOC, GPIO_Pin_7);
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
			break;
		case CLOSE:
			GPIO_SetBits(GPIOC,GPIO_Pin_8);
			GPIO_ResetBits(GPIOC,GPIO_Pin_7);
			break;
		case OFF:
			GPIO_SetBits(GPIOC,GPIO_Pin_7);
			GPIO_SetBits(GPIOC,GPIO_Pin_8);
			break;
	}
	
}

