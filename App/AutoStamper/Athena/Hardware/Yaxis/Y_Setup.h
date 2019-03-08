#ifndef __Y_SETUP_H
#define __Y_SETUP_H


#include "tim_3.h"


// ���Ʒ���
#define Y_DIR_CLK       RCC_APB2Periph_GPIOA
#define Y_DIR_PIN       GPIO_Pin_3
#define Y_DIR_PORT      GPIOA
#define Y_DIR_SET       GPIO_SetBits(Y_DIR_PORT, Y_DIR_PIN)
#define Y_DIR_RESET     GPIO_ResetBits(Y_DIR_PORT, Y_DIR_PIN)


//ʹ��������
#define Y_ENA_CLK       RCC_APB2Periph_GPIOA
#define Y_ENA_PIN       GPIO_Pin_5
#define Y_ENA_PORT      GPIOA
#define Y_ENA_SET       GPIO_SetBits(Y_ENA_PORT, Y_ENA_PIN)
#define Y_ENA_RESET     GPIO_ResetBits(Y_ENA_PORT, Y_ENA_PIN)

//��ʱ������PUL
#define Y_TIMx				 TIM3
#define Y_TIM_SetAutoreload  TIM_SetAutoreload 
#define Y_TIM_SetCompare2    TIM_SetCompare2

#define CW      0
#define CCW     1

#define STOP     0
#define ACCEL    1
#define DECEL    2
#define RUN      3

#define Y_TIM_PRESCALER       8                                 // ��ʱ��Ԥ��Ƶ
#define T3_FREQ     (SystemCoreClock/(Y_TIM_PRESCALER + 1))     // ��ʱ��Ƶ��
#define SPR         200                                         // ���������תһȦ���������������
#define MICRO_STEP  4                                           // ���������ϸ����
#define Y_STEP_S		50


#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // ÿ������Ļ��ȣ������
#define Y_A_T         ((float)(ALPHA*T3_FREQ))

typedef struct{
	uint8_t dir;                        	// ����
	int16_t total_count;					// �ܼ���
	int16_t step_arr;                 		// arrֵ����������ļ�������
	uint32_t decel_start;               	// ���ٿ�ʼ��������
	int32_t accel_count;                	// ���ٹ��̵�������
	uint8_t run_state;                 		// ����״̬
}YspeedRampData;


double exp(double x);

void TIM3_CH2_Init(void);
static void Y_DIR_ENA_Init(void);
void Y_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);

void Y_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible);
static void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible);



#endif
