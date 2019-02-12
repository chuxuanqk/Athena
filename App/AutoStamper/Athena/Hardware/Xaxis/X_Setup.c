#include "X_Setup.h"


XspeedRampData srd_x; 
int32_t X_Status = 0;           // 是否在运动？ 0：停止， 1：运动
int32_t xpos = 0;               // 当前位置

int32_t flag = 1;


double exp(double x)
{
    uint8_t i = 0;

    x = 1.0 + x/256;
    for(; i<6; i++)
    {
        x = x*x;
    }
    return x;
}


void TIM2_CH2_Init(uint16_t arr, uint16_t psc)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);        // 使能TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // 使能GPIOA

    TIM2_RemapConfig_Init();                    // 配置重映射
    GPIOA_1_CH2_Init();                         // 初始化GPIOA_1
    TIM2_NVIC_Init();                           // 初始化TIM2中断配置
    TIM2_Init(arr, psc);                        // 初始化TIM2
    TIM2_PWM_Init();                            // 配置TIM2的PWM模式
    
    TIM_SetAutoreload(TIM2, arr);               // 设置自动重装载值
	TIM_SetCompare1(TIM2, arr/2);               // 设置比较值

    TIM_Cmd(TIM2, ENABLE);                      // 使能TIM2
}


/*
函数功能：电机使能
*/
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
				X_ENA_SET;
			break;
	}
}


void CalculateSModelLine(float fre[], uint16_t arr[], uint16_t len, float fre_max, float fre_min, float flexible)
{
    uint32_t i=0;
    float x;
    float y;
    float D_value = fre_max - fre_min;

    for(; i<len; i++)
    {
        x = flexible*(i-len/2)/(len/2);                 //归一化
        y = 1.0/(1+exp(-x));
        fre[i] = D_value*y + fre_min;
        arr[i] = (uint16_t)((T1_FREQ/fre[i]) - 1);
    }

    return;
}

/********************************************************
 * step:  移动的步数，正为顺时针，负为逆时针
 * speed: 最大角速度(rad/s)
 * 800脉冲一圈
 * ******************************************************/
void X_MoveAbs(int32_t step, uint16_t len,float fre_max, float fre_min, uint32_t speed, float flexible)
{
    float fre[len] = {0};
    float arr[len] = {0};

    CalculateSModelLine(fre, arr, len, fre_max, fre_min, 8);

    step = step - X_pos;         // 绝对位移

    X_ENA(TIM2, ENABLE);

    // 设置运动方向
    if(step < 0)
    {
        srd_x.dir = CCW;
        X_DIR_RESET;
        step = -step; 
    } else{
        srd_x.dir = CW;
        X_DIR_SET;
    }

    // 当旋转角度很小时，直接以启动频率开始运行
    if(step == 1)
    {
        srd_x.accel_count = 1;
        srd_x.step_delay = int16_t((T1_FREQ/fre_min) - 1);
        srd_x.run_state = DECEL;
        X_Status = 1;
    }else if(step != 0){
        if(step < 2*len)
        {
            srd_x.step_delay = arr[0];
            srd_x.accel_count = int16_t(step/2);
            srd_x.run_state = ACCEL;
            X_Status = 1;
        }else{
            srd_x.step_delay = arr[0];
            srd_x.accel_count = len;
            srd_x.run_state = ACCEL;
            X_Status = 1;
        }
    }

}

