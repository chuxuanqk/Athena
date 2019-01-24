#include "led.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "tim_2.h"


int main(void)
{
	delay_init();
	NVIC_Configuration();
	LED_Init();
	EXTIX_Init();
	TIM2_PWM_Init(39, 71);           //pscԤ��Ƶ8��TIM2ʱ��Ƶ��8kHz;arr=7999,PWM����Ϊ1/1000��

	LED = 0;
	
	while(1);
}

