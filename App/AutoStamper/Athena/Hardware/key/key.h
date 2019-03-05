#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

// #define S1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)          //读取按键S1
// #define S2 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)          //读取按键S2
// #define S3 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)          //读取按键S3

#define S1 PEin(4)         //读取按键S1
#define S2 PEin(3)         //读取按键S2
#define S3 PEin(2)         //读取按键S3

void KEY_Init(void);

void EXTI2_Init(void);
void EXTI3_Init(void);
void EXTI4_Init(void);

void NVIC2_Init(void);
void NVIC3_Init(void);
void NVIC4_Init(void);

void EXTIX_Init(void);

void stop(void);

extern uint32_t STEP;
extern uint8_t FLAG;

#endif
