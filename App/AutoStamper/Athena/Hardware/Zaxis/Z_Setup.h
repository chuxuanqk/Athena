#ifndef __Z_SETUP_H
#define __Z_SETUP_H


#include "tim_4.h"


// ���Ʒ���DIR
#define Z_DIR_CLK       RCC_APB2Periph_GPIOB
#define Z_DIR_PIN       GPIO_Pin_5
#define Z_DIR_PORT      GPIOB
#define Z_DIR_SET       GPIO_SetBits(Z_DIR_PORT, Z_DIR_PIN)
#define Z_DIR_RESET     GPIO_ResetBits(Z_DIR_PORT, Z_DIR_PIN)

//ʹ��������ENA
#define Z_ENA_CLK       RCC_APB2Periph_GPIOB
#define Z_ENA_PIN       GPIO_Pin_6
#define Z_ENA_PORT      GPIOB
#define Z_ENA_SET       GPIO_SetBits(Z_ENA_PORT, Z_ENA_PIN)
#define Z_ENA_RESET     GPIO_ResetBits(Z_ENA_PORT, Z_ENA_PIN)

//��ʱ������PUL
#define Z_TIMx				 TIM4
#define Z_TIM_SetAutoreload  TIM_SetAutoreload 
#define Z_TIM_SetCompare3    TIM_SetCompare2


#define CW      0
#define CCW     1

#define STOP     0
#define ACCEL    1
#define DECEL    2
#define RUN      3

#define Z_TIM_PRESCALER       8                                 // ��ʱ��Ԥ��Ƶ
#define T4_FREQ     (SystemCoreClock/(Z_TIM_PRESCALER + 1))     // ��ʱ��Ƶ��
#define SPR         200                                         // ���������תһȦ���������������
#define MICRO_STEP  4                                           // ���������ϸ����
#define STEP_S		800


#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // ÿ������Ļ��ȣ������
#define Z_A_T         ((float)(ALPHA*T4_FREQ))

typedef struct{
	uint8_t dir;                        	// ����
	int16_t total_count;					// �ܼ���
	int16_t step_arr;                 		// arrֵ����������ļ�������
	uint32_t decel_start;               	// ���ٿ�ʼ��������
	int32_t accel_count;                	// ���ٹ��̵�������
	uint8_t run_state;                 		// ����״̬
}ZspeedRampData;


double exp(double x);

void TIM4_CH2_Init(void);
static void Z_DIR_ENA_Init(void);
void Z_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);

void Z_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible);
static void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible);



#endif
