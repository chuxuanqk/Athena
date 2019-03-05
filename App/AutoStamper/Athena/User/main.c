#include "led.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "tim_2.h"
#include "X_Setup.h"
#include "catch.h"
#include "plug_in.h"


int32_t set_Xstep,set_Ystep,set_accel,set_decel,set_speed;   
int32_t Xstep,Ystep ;
int16_t Status;
uint32_t accel = 9,decel = 9,speed = 9;


// X轴测试模块
void Xstep_Test(void)
{
	TIM2_CH2_Init();
	
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{			
			Xstep = (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
			Xstep = Xstep*800;
			
			set_speed = speed*60/9.55;
			set_accel = accel*2*3.14159;
			set_decel = decel*2*3.14159;
			// set_Xstep = Xstep*800/33.5;          // 距离
			
			set_Xstep = (uint32_t)(Xstep/(33.5*ALPHA));     // 步数

			printf("Xstep:%d\r\n", Xstep);

			X_MoveAbs(Xstep, 12800.0, 800.0,5.0);
			
			USART_RX_STA=0;
		}
		
	}
}

void Xstep_Key_Test(void)
{
	TIM2_CH2_Init();
	
	while(1)
	{
		if(FLAG == 1)
		{
			LED = !LED;
			X_MoveAbs(STEP, 12800.0, 800.0,5.0);
			FLAG = 0;
		}
	}
}



// 抓手测试模块
void Catch_Test(void)
{
	Catch_GPIO_Init();

	while(1)
	{
		if(USART_RX_STA&0x8000)
		{
			
			Status = (USART_RX_BUF[0]-48);
			printf("Status:%d\r\n", Status);
			switch(Status)
			{
				case 0:
					Catch(OPEN);
					break;
				
				case 1:
					Catch(CLOSE);
					break;
					
				case 2:
					Catch(OFF);
					break;
				default:
					break;
			}
			USART_RX_STA=0;
		}
	}

}



int main(void)
{
	delay_init();
	NVIC_Configuration();
	
	uart_init(9600);
	LED_Init();
	EXTIX_Init();
	
	Xstep_Key_Test();

}

