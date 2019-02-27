#ifndef __CATCH_H
#define __CATCH_H

#include "stm32f10x.h"
// #include "tim8.h"

typedef enum 
{
	OPEN,
	CLOSE,
	OFF
}sw;

//CATCH(ON)   抓取印章
//CATCH(OFF)  松开印章
//继电器控制抓手电机，高电平触发

void Catch_GPIO_Init(void); 
void Catch(sw SW);      


#endif
