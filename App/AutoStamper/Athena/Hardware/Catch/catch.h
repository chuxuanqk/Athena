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

//CATCH(ON)   ץȡӡ��
//CATCH(OFF)  �ɿ�ӡ��
//�̵�������ץ�ֵ�����ߵ�ƽ����

void Catch_GPIO_Init(void); 
void Catch(sw SW);      


#endif
