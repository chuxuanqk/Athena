#include "led.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "tim_2.h"
//#include "Xaxis_setup.h"
#include "X_Setup.h"


int32_t set_Xstep,set_Ystep,set_accel,set_decel,set_speed;   
int32_t Xstep,Ystep ;
uint32_t accel = 9,decel = 9,speed = 9;


int main(void)
{
	delay_init();
	NVIC_Configuration();
	
	uart_init(9600);
	//LED_Init();
	//EXTIX_Init();
	//TIM2_PWM_Init(39, 71);           //psc预分频8，TIM2时钟频率8kHz;arr=7999,PWM周期为1/1000；
	
	//X_TIM_init();
	
	TIM2_CH2_Init();

	//LED = 0;
	
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{			
			Xstep = (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
			Xstep = Xstep*2;
			
			set_speed = speed*60/9.55;
			set_accel = accel*2*3.14159;
			set_decel = decel*2*3.14159;
			// set_Xstep = Xstep*800/33.5;          // 距离
			
			set_Xstep = (uint32_t)(Xstep/(33.5*ALPHA));     // 步数

			//printf("Xstep:%d\r\n", Xstep);

			X_MoveAbs(Xstep, 12800.0, 800.0,5.0);
			
			USART_RX_STA=0;
		}
		
	}
}

