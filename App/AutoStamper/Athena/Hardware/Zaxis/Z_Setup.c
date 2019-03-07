#include "Z_Setup.h"
#include "usart.h"

ZspeedRampData srd_z; 
int32_t Z_Status = 0;           // 是否在运动？ 0：停止， 1：运动
uint32_t Z_pos = 0;               // 当前位置

static float __FRE[STEP_S] = {0.0};
static uint16_t __ARR[STEP_S] = {0};


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



void TIM4_CH2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);

    Z_DIR_ENA_Init();

    GPIOB_7_CH2_Init();
    TIM4_NVIC_Init();
    TIM4_Init(9999, 8);
    TIM4_PWM_Init();

    TIM_OC4PreloadConfig(Z_TIMx, TIM_OCPreload_Enable);
    TIM_ClearFlag(Z_TIMx, TIM_FLAG_Update);
    TIM_ITConfig(Z_TIMx, TIM_IT_Update, ENABLE);

    TIM_Cmd(Z_TIMx, DISABLE);
    
}


static void Z_DIR_ENA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(Z_DIR_CLK ,ENABLE);

    GPIO_InitStructure.GPIO_Pin = Z_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(Z_DIR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Z_ENA_PIN;
    GPIO_Init(Z_ENA_PORT, &GPIO_InitStructure);

	Z_DIR_RESET;
	Z_ENA_RESET;
	//Z_ENA_SET;
}

void Z_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState)
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
			if(MOTOx == TIM4)
				Z_ENA_RESET;
			break;
		case 0:
			if(MOTOx == TIM4)
				Z_ENA_SET;
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
        x = flexible*(i-(len/2))/(len/2);                 //归一化
        y = 1.0/(1+exp(-x));
		
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T4_FREQ/fre[i]) -  1);
    }
	
    return;
}



/********************************************************
 * step:  移动的步数，正为顺时针，负为逆时针
 * speed: 最大角速度(rad/s)
 * 800脉冲一圈
 * ******************************************************/
void Z_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible)
{
    CalculateSModelLine(__FRE, __ARR, STEP_S, fre_max, fre_min, flexible);

    step = step - Z_pos;         // 绝对位移
	
    Z_ENA(Z_TIMx, ENABLE);

    // 设置运动方向
    if(step < 0)
    {
        srd_z.dir = CCW;
		Z_DIR_RESET;
        
		step = -step; 
    }else{
        srd_z.dir = CW;
		Z_DIR_SET;                   // step为正，ENA为+，顺时针
    }
	
	srd_z.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
    if(step == 1)
    {
        srd_z.accel_count = 1;
        srd_z.step_arr = (int16_t)((T4_FREQ/fre_min) - 1);
        srd_z.run_state = DECEL;
        Z_Status = 1;
    }else if(step != 0){
        if(step <= (STEP_S*2))
        {
            srd_z.step_arr = __ARR[0];
            srd_z.accel_count = (int32_t)(step/2);
            srd_z.decel_start = srd_z.accel_count;
            srd_z.run_state = ACCEL;
            Z_Status = 1;
        }else{
            srd_z.step_arr = __ARR[0];
            srd_z.accel_count = STEP_S;
            srd_z.decel_start = step - STEP_S;
            srd_z.run_state = ACCEL;
            Z_Status = 1;
        }
    }
	
    Z_TIM_SetAutoreload(Z_TIMx, srd_z.step_arr);
    Z_TIM_SetAutoreload(Z_TIMx, (srd_z.step_arr/2));

    TIM_Cmd(Z_TIMx, ENABLE);                    // 定时器使能
}


/***************************************
 * 
 *           TIM3中断处理函数
 * 
 * ************************************/
void TIM4_IRQHandler(void)
{
    static uint16_t step_count = 0;                 // 总移步数计数器
	
    if(TIM_GetITStatus(Z_TIMx, TIM_IT_Update) != RESET)
    {
        switch(srd_z.run_state)
        {
            case STOP:
                step_count = 0;

                TIM_Cmd(Z_TIMx, DISABLE);
                Z_ENA_RESET;
                Z_Status = 0;

                break;

            case ACCEL:
                step_count++;
			
                if(srd_z.dir==CW)
                {
                    Z_pos++;
                }else{
                    Z_pos--;
                }

                if(step_count < srd_z.accel_count)
                {
                    srd_z.step_arr = __ARR[step_count];
                    srd_z.run_state = ACCEL;
					
                }else if(step_count >= srd_z.decel_start){                          // 直接进入减速阶段
                    srd_z.step_arr = __ARR[step_count-1];
					
                    srd_z.run_state = DECEL;
					
                }else if(step_count >= STEP_S){    
					// 进入匀速阶段
                    srd_z.step_arr = __ARR[STEP_S-1];
                    srd_z.run_state = RUN;
                }
				
                break;

            case RUN:
                step_count++;
			
                if(srd_z.dir==CW)
                {
                    Z_pos++;
                }else{
                    Z_pos--;
                }

                if(step_count >= srd_z.decel_start)
                {
                    srd_z.step_arr = __ARR[STEP_S-1];
                    srd_z.run_state = DECEL;
                }
                break;

            case DECEL:
                step_count++;
			
                if(srd_z.dir==CW)
                {
                    Z_pos++;
                }else{
                    Z_pos--;
                }
                
                if(step_count < srd_z.total_count)
                {
                    srd_z.step_arr = __ARR[srd_z.total_count-step_count];
                }else{
					srd_z.run_state = STOP;
                }
                break;

        }
        Z_TIM_SetAutoreload(Z_TIMx, srd_z.step_arr);
        Z_TIM_SetCompare3(Z_TIMx, (srd_z.step_arr/2));

        TIM_ClearITPendingBit(Z_TIMx, TIM_IT_Update);
    }
}

