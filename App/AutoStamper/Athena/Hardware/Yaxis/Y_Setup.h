#ifndef __Y_SETUP_H
#define __Y_SETUP_H


#include "tim_3.h"


// 控制方向
#define Y_DIR_CLK       RCC_APB2Periph_GPIOA
#define Y_DIR_PIN       GPIO_Pin_3
#define Y_DIR_PORT      GPIOA
#define Y_DIR_SET       GPIO_SetBits(Y_DIR_PORT, Y_DIR_PIN)
#define Y_DIR_RESET     GPIO_ResetBits(Y_DIR_PORT, Y_DIR_PIN)


//使能驱动器
#define Y_ENA_CLK       RCC_APB2Periph_GPIOA
#define Y_ENA_PIN       GPIO_Pin_5
#define Y_ENA_PORT      GPIOA
#define Y_ENA_SET       GPIO_SetBits(Y_ENA_PORT, Y_ENA_PIN)
#define Y_ENA_RESET     GPIO_ResetBits(Y_ENA_PORT, Y_ENA_PIN)

//定时器脉冲PUL
#define Y_TIMx				 TIM3
#define Y_TIM_SetAutoreload  TIM_SetAutoreload 
#define Y_TIM_SetCompare2    TIM_SetCompare2

#define CW      0
#define CCW     1

#define STOP     0
#define ACCEL    1
#define DECEL    2
#define RUN      3

#define Y_TIM_PRESCALER       8                                 // 定时器预分频
#define T3_FREQ     (SystemCoreClock/(Y_TIM_PRESCALER + 1))     // 定时器频率
#define SPR         200                                         // 步进电机旋转一圈脉冲数，电机常数
#define MICRO_STEP  4                                           // 电机驱动器细分数
#define Y_STEP_S		50


#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // 每个脉冲的弧度，步距角
#define Y_A_T         ((float)(ALPHA*T3_FREQ))

typedef struct{
	uint8_t dir;                        	// 方向
	int16_t total_count;					// 总计数
	int16_t step_arr;                 		// arr值，单个脉冲的计数个数
	uint32_t decel_start;               	// 减速开始的脉冲数
	int32_t accel_count;                	// 加速过程的脉冲数
	uint8_t run_state;                 		// 运行状态
}YspeedRampData;


double exp(double x);

void TIM3_CH2_Init(void);
static void Y_DIR_ENA_Init(void);
void Y_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);

void Y_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible);
static void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible);



#endif
