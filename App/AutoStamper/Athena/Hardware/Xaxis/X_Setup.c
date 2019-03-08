#include "X_Setup.h"
#include "usart.h"

XspeedRampData srd_x; 
int32_t X_Status = 0;           // �Ƿ����˶��� 0��ֹͣ�� 1���˶�
uint32_t X_pos = 0;               // ��ǰλ��


static float __FRE[X_STEP_S] = {0.0};
static uint16_t __ARR[X_STEP_S] = {0};

static double exp(double x)
{
    uint16_t i = 0;

    x = 1.0 + x/256;
    for(; i<8; i++)
    {
        x = x*x;
    }
    return x;
}


void TIM2_CH2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);        // ʹ��TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // ʹ��GPIOA

	DIR_ENA_Init();									// ��ʼ��DIR_ENA

    TIM2_RemapConfig_Init();                        // ������ӳ��
    GPIOA_1_CH2_Init();                             // ��ʼ��GPIOA_1
    TIM2_NVIC_Init();                               // ��ʼ��TIM2�ж�����
    TIM2_Init(9999, 8);                             // ��ʼ��TIM2
    TIM2_PWM_Init();                                // ����TIM2��PWMģʽ
    
    TIM_OC2PreloadConfig(X_TIMx, TIM_OCPreload_Enable);             // ʹ����װ��
    TIM_ClearFlag(X_TIMx, TIM_FLAG_Update);                         // ����FLAG����
    TIM_ITConfig(X_TIMx, TIM_IT_Update, ENABLE);                    // ʹ��ͨ�ö�ʱ���ж�

    //TIM_SetAutoreload(TIM2, 19999);               // �����Զ���װ��ֵ
	//TIM_SetCompare1(TIM2, 9999);               // ���ñȽ�ֵ

	TIM_Cmd(X_TIMx, DISABLE);
    //TIM_Cmd(X_TIMx, ENABLE);                      // ʹ��TIM2
}


/********************************************
*    �������ܣ�����������ʹ�ܶ˿ڳ�ʼ��
********************************************/
static void DIR_ENA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(X_DIR_CLK ,ENABLE);

    GPIO_InitStructure.GPIO_Pin = X_DIR_PIN|X_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(X_DIR_PORT, &GPIO_InitStructure);

	X_DIR_RESET;
	X_ENA_RESET;
	//X_ENA_SET;
	
}



/*******************************************
*           �������ܣ����ʹ��
*******************************************/
void X_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState)
{
	u16 Ena = 0;
	assert_param(IS_TIM_LIST6_PERIPH(MOTOx));
	if(NewState == ENABLE)
	{
		Ena = 1;
	}
	switch(Ena)
	{
		case 1:
			if(MOTOx == TIM2)
				X_ENA_RESET;
			break;
		case 0:
			if(MOTOx == TIM2)
				//X_ENA_SET;
			break;
	}
}


static void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible)
{
    int32_t i=0;
    float x = 0.0;
    float y = 0;
    float D_value = fre_max - fre_min;
	
    for(; i<len; i++)
    {
        x = flexible*(i-(len/2))/(len/2);                 //��һ��
        y = 1.0/(1+exp(-x));
		
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T1_FREQ/fre[i]) -  1);
    }
	
	
    return;
}

/********************************************************
 * step:  �ƶ��Ĳ�������Ϊ˳ʱ�룬��Ϊ��ʱ��
 * speed: �����ٶ�(rad/s)
 * 800����һȦ
 * ******************************************************/
void X_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible)
{
    CalculateSModelLine(__FRE, __ARR, X_STEP_S, fre_max, fre_min, flexible);
  
    step = step - X_pos;         // ����λ��
	
    X_ENA(X_TIMx, ENABLE);

    // �����˶�����
    if(step < 0)
    {
        srd_x.dir = CCW;
		X_DIR_RESET;
        
		step = -step; 
    }else{
		//printf("step>0\r\n");
        srd_x.dir = CW;
		X_DIR_SET;                   // stepΪ����ENAΪ+��˳ʱ��
    }

	srd_x.total_count = step;    // ��¼Ҫ�ߵ��ܲ���

    // ����ת�ǶȺ�Сʱ��ֱ��������Ƶ�ʿ�ʼ����
    if(step == 1)
    {
        srd_x.accel_count = 1;
        srd_x.step_arr = (int16_t)((T1_FREQ/fre_min) - 1);
        srd_x.run_state = DECEL;
        X_Status = 1;
    }else if(step != 0){
        if(step <= (X_STEP_S*2))
        {
            srd_x.step_arr = __ARR[0];
            srd_x.accel_count = (int32_t)(step/2);
            srd_x.decel_start = srd_x.accel_count;
            srd_x.run_state = ACCEL;
            X_Status = 1;
			
        }else{
            srd_x.step_arr = __ARR[0];
            srd_x.accel_count = X_STEP_S;
            srd_x.decel_start = step - X_STEP_S;
            srd_x.run_state = ACCEL;
            X_Status = 1;
        }
    }
    X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
    X_TIM_SetAutoreload(X_TIMx, (srd_x.step_arr/2));

    TIM_Cmd(X_TIMx, ENABLE);                    // ��ʱ��ʹ��
}


/***************************************
 * 
 *           TIM2�жϴ�����
 * 
 * ************************************/
void TIM2_IRQHandler(void)
{
    static uint16_t step_count = 0;                 // ���Ʋ���������

    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
        switch(srd_x.run_state)
        {
            case STOP:	
                step_count = 0;

                TIM_Cmd(X_TIMx, DISABLE);
                X_ENA_RESET;
                X_Status = 0;

                break;

            case ACCEL:
                step_count++;
			
                if(srd_x.dir==CW)
                {
                    X_pos++;
                }else{
                    X_pos--;
                }

                if(step_count < srd_x.accel_count)
                {
                    srd_x.step_arr = __ARR[step_count];
                    srd_x.run_state = ACCEL;
					
                }else if(step_count >= srd_x.decel_start){                          // ֱ�ӽ�����ٽ׶�
                    srd_x.step_arr = __ARR[step_count-1];
                    srd_x.run_state = DECEL;
					
                }else if(step_count >= X_STEP_S){    
					// �������ٽ׶�
                    srd_x.step_arr = __ARR[X_STEP_S-1];
                    srd_x.run_state = RUN;
                }
				
                break;

            case RUN:
                step_count++;
			
                if(srd_x.dir==CW)
                {
                    X_pos++;
                }else{
                    X_pos--;
                }

                if(step_count >= srd_x.decel_start)
                {
                    srd_x.step_arr = __ARR[X_STEP_S-1];
                    srd_x.run_state = DECEL;
                }
                
                break;

            case DECEL:
                step_count++;
			
                if(srd_x.dir==CW)
                {
                    X_pos++;
                }else{
                    X_pos--;
                }
                
                if(step_count < srd_x.total_count)
                {
                    srd_x.step_arr = __ARR[srd_x.total_count-step_count];
                }else{				
					srd_x.run_state = STOP;
                }

                break;
        }
        X_TIM_SetAutoreload(X_TIMx, srd_x.step_arr);
        X_TIM_SetCompare2(X_TIMx, (srd_x.step_arr/2));

        TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
    }

}



