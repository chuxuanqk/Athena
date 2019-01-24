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
	TIM2_PWM_Init(39, 71);           //psc预分频8，TIM2时钟频率8kHz;arr=7999,PWM周期为1/1000；

	LED = 0;
	
	while(1);
}

