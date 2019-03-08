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
int16_t Status;
uint32_t accel = 9,decel = 9,speed = 9;
int32_t Xstep,Ystep ;

//float FRE_MAX = 12800.0;
//float FRE_MIN = 800.0;
//float FLEXIBLE = 5.0;


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

			X_MoveAbs(Xstep, FRE_MAX, FRE_MIN, FLEXIBLE);
			
			USART_RX_STA=0;
		}
		
	}
}

void Xstep_Key_Test(void)
{
	uint32_t X_STEP;
	uint32_t Y_STEP;
	uint32_t X_statu;
	uint32_t Y_statu;
	
	uint32_t mul = 25;
	
	TIM2_CH2_Init();
	TIM3_CH2_Init();
	
	while(1)
	{
//		if(FLAG == 1)
//		{
//			LED = !LED;
//			X_MoveAbs(STEP, 12800.0, 800.0,5.0);
//			Y_MoveAbs(STEP, 12800.0, 800.0,5.0);
//			FLAG = 0;
//		}
		

		
		if(USART_RX_STA&0x8000)
		{

			X_STEP  =  (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
			Y_STEP  =  (USART_RX_BUF[3]-48)*100+(USART_RX_BUF[4]-48)*10+(USART_RX_BUF[5]-48);
			X_statu =  USART_RX_BUF[6]-48;
			Y_statu =  USART_RX_BUF[7]-48;
			
			X_STEP = mul*X_STEP;
			Y_STEP = mul*Y_STEP;
			
			if(X_statu == 0)
			{
				X_STEP = -X_STEP;
			}else{
				X_STEP = X_STEP;
			}
			if(Y_statu == 0)
			{
				Y_STEP = -Y_STEP;
			}else{
				Y_STEP = Y_STEP;
			}
			printf("X_STEP:%d\r\n", X_STEP);
			printf("Y_STEP:%d\r\n", Y_STEP);
			
			X_MoveAbs(X_STEP, 8000.0, 800.0,5.0);
			Y_MoveAbs(Y_STEP, 8000.0, 800.0,5.0);
				

			USART_RX_STA=0;
		}
	}
}


void Ystep_Key_Test(void)
{
	TIM3_CH2_Init();
	
	while(1)
	{
		if(FLAG == 1)
		{
			LED = !LED;
			Y_MoveAbs(STEP, 12800.0, 800.0,5.0);
			FLAG = 0;
		}
	}
}

void Zstep_Key_Test(void)
{
	TIM4_CH2_Init();
	
	while(1)
	{
		if(FLAG == 1)
		{
			LED = !LED;
			Z_MoveAbs(STEP, 12800.0, 800.0,5.0);
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
	//Ystep_Key_Test();
	//Zstep_Key_Test();
}

