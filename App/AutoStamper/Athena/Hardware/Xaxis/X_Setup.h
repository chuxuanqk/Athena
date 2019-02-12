#ifndef __X_SETUP_H
#define __X_SETUP_H

#include "tim_2.h"

// ���Ʒ���
#define X_DIR_CLK       RCC_APB2Periph_GPIOC
#define X_DIR_PIN       GPIO_Pin_1
#define X_DIR_PORT      GPIOC
#define X_DIR_SET       GPIO_SetBits(X_DIR_PORT, X_DIR_PIN)
#define X_DIR_RESET     GPIO_ResetBits(X_DIR_PORT, X_DIR_PIN)

//ʹ��������
#define X_ENA_CLK       RCC_APB2Periph_GPIOC
#define X_ENA_PIN       GPIO_Pin_3
#define X_ENA_PORT      GPIOC
#define X_ENA_SET       GPIO_SetBits(X_ENA_PORT, X_ENA_PIN)
#define X_ENA_RESET     GPIO_ResetBits(X_ENA_PORT, X_ENA_PIN)


void TIM2_CH2_Init(uint16_t arr, uint16_t psc);
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState);


double exp(double x);
void X_MoveAbs(int32_t step, uint16_t len,float fre_max, float fre_min, uint32_t speed, float flexible);
void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible);


#define CW      0
#define CCW     1

#define STOP     0
#define ACCEL    1
#define DECEL    2
#define RUN      3

#define X_TIM_PRESCALER       8                                 // ��ʱ��Ԥ��Ƶ
#define T1_FREQ     (SystemCoreClock/(X_TIM_PRESCALER + 1))     // ��ʱ��Ƶ��
#define SPR         200                                         // ���������תһȦ���������������
#define MICRO_STEP  4                                           // ���������ϸ����

#define ALPHA       ((float)((2*3.1415*100/SPR)/100))                     // ÿ������Ļ��ȣ������
#define A_T         ((float)(ALPHA*T1_FREQ))


typedef struct{
	uint8_t run_state;                  // ����״̬
	uint8_t dir;                        // ����
	int16_t step_delay;                 // arrֵ����������ļ�������
	uint32_t decel_start;               // ���ٿ�ʼ��������
	//int32_t decel_val;                  // ����ֵ
	int32_t min_delay;
	int32_t accel_count;                //���ٹ��̵�������
}XspeedRampData;


#endif
