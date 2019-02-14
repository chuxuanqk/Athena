#ifndef __X_SETUP_H
#define __X_SETUP_H

#include "tim_2.h"

// 控制方向
#define X_DIR_CLK       RCC_APB2Periph_GPIOC
#define X_DIR_PIN       GPIO_Pin_1
#define X_DIR_PORT      GPIOC
#define X_DIR_SET       GPIO_SetBits(X_DIR_PORT, X_DIR_PIN)
#define X_DIR_RESET     GPIO_ResetBits(X_DIR_PORT, X_DIR_PIN)

//使能驱动器
#define X_ENA_CLK       RCC_APB2Periph_GPIOC
#define X_ENA_PIN       GPIO_Pin_3
#define X_ENA_PORT      GPIOC
#define X_ENA_SET       GPIO_SetBits(X_ENA_PORT, X_ENA_PIN)
#define X_ENA_RESET     GPIO_ResetBits(X_ENA_PORT, X_ENA_PIN)

#define X_TIMx				 TIM2
#define X_TIM_SetAutoreload  TIM_SetAutoreload 
#define X_TIM_SetCompare2    TIM_SetCompare2

void TIM2_CH2_Init(void);
void DIR_ENA_Init(void);
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);


double exp(double x);
void X_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible);
void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible);
    

#define CW      0
#define CCW     1

#define STOP     0
#define ACCEL    1
#define DECEL    2
#define RUN      3

#define X_TIM_PRESCALER       8                                 // 定时器预分频
#define T1_FREQ     (SystemCoreClock/(X_TIM_PRESCALER + 1))     // 定时器频率
#define SPR         200                                         // 步进电机旋转一圈脉冲数，电机常数
#define MICRO_STEP  4                                           // 电机驱动器细分数
#define STEP_S		800


#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // 每个脉冲的弧度，步距角
#define A_T         ((float)(ALPHA*T1_FREQ))


extern int8_t flag;

typedef struct{
	uint8_t dir;                        	// 方向
	int16_t total_count;					// 总计数
	int16_t step_arr;                 		// arr值，单个脉冲的计数个数
	uint32_t decel_start;               	// 减速开始的脉冲数
	int32_t accel_count;                	// 加速过程的脉冲数
	uint8_t run_state;                 		// 运行状态
}XspeedRampData;


#endif
