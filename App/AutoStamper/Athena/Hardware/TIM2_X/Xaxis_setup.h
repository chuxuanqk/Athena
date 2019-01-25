#ifndef __XAXIS_SETUP_H__
#define __XAXIS_SETUP_H__

#include "stm32f10x_conf.h"

#define X_PULSE_CLK                 RCC_APB2Periph_GPIOA
#define X_PULSE_PIN                 GPIO_Pin_1        // 脉冲输出引脚
#define X_PULSE_PORT                GPIOA

#define X_DIR_CLK                   RCC_APB2Periph_GPIOC
#define X_DIR_PIN                   GPIO_Pin_1        // 方向控制引脚
#define X_DIR_ENA_PORT              GPIOC
#define X_DIR_CW()                  GPIO_SetBits(AxisDirPort, AXIS_DIR_1)
#define X_DIR_CCW()                 GPIO_ResetBits(AxisDirPort, AXIS_DIR_1)

#define X_ENA_PIN                   GPIO_Pin_0                

#define X_TIM_CLOCKCMD              RCC_APB1PeriphClockCmd
#define X_TIM_RCC_CLK               RCC_APB1Periph_TIM2
#define X_TIMx                      TIM2
#define X_TIM_IRQn                  TIM2_CC_IRQn       //还不熟悉
#define X_TIM_OCnInit               TIM_OC2Init        //还不熟悉 
#define X_TIM_SetCompare2           TIM_SetCompare2    //还不熟悉



void X_TIM_init(void);
uint32_t X_sqrt(uint32_t x);
static void X_GPIO_init(void);
static void X_NVIC_init(void);
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);
void X_MoveAbs(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);





#define X_TIM_PRESCALER                      8 //实际则是 8MHz
#define X_TIM_PERIOD                         0xFFFF
/* 加减速算法结构体与其它变量*/

typedef struct{
      uint8_t run_state; // 运行状态
			uint8_t dir;
			int32_t step_delay;
			uint32_t decel_start;
			int32_t decel_val;//减速值
			int32_t min_delay;
			int32_t accel_count;                 //加速过程的脉冲数

}XspeedRampData;

#define FALSE   0
#define TRUE    1
#define CW      0
#define CCW     1

#define STOP     0
#define ACCEL    1
#define DECEL    2
#define RUN      3

#define T1_FREQ   (SystemCoreClock/(X_TIM_PRESCALER+1))// 定时器频率
#define SPR        200 //STEPS PER ROUND 
#define MICRO_STEP   4// 细分数
#define PPR        (SPR*MICRO_STEP) //Pulse per second 一圈需要的脉冲数

#define ALPHA       ((float)(2*3.1415/PPR)) //每个脉冲的弧度
#define A_T_x10      ((float)(ALPHA*T1_FREQ)) //定时器频率与alpha的20倍数
#define T1_FREQ_148   ((float)((0.676*T1_FREQ))) //修正参数代入
#define A_SQ          ((float)(2*ALPHA)) //放大倍数平衡参数
// 这个量在公式当中没有出现过#define A_x200


extern int32_t X_Status;
extern int32_t flag;


















#endif	/* __XAXIS_SETUP_H__ */
