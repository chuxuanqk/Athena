#include "Y_Setup.h"
#include "usart.h"

static YspeedRampData srd_y; 
int32_t Y_Status = 0;           // 是否在运动？ 0：停止， 1：运动
static uint32_t Y_pos = 0;               // 当前位置

static float __FRE[Y_STEP_S] = {0.0};
static uint16_t __ARR[Y_STEP_S] = {0};


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


void TIM3_CH2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

    Y_DIR_ENA_Init();

    GPIOA_7_CH2_Init();
    TIM3_NVIC_Init();
    TIM3_Init(9999, 8);
    TIM3_PWM_Init();

    TIM_OC3PreloadConfig(Y_TIMx, TIM_OCPreload_Enable);
    TIM_ClearFlag(Y_TIMx, TIM_FLAG_Update);
    TIM_ITConfig(Y_TIMx, TIM_IT_Update, ENABLE);

    TIM_Cmd(Y_TIMx, DISABLE);
    
}

static void Y_DIR_ENA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(Y_DIR_CLK ,ENABLE);

    GPIO_InitStructure.GPIO_Pin = Y_DIR_PIN|Y_ENA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(Y_DIR_PORT, &GPIO_InitStructure);

	Y_DIR_RESET;
	Y_ENA_RESET;
	//Y_ENA_SET;
}


void Y_ENA(TIM_TypeDef* MOTOx, FunctionalState NewState)
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
			if(MOTOx == TIM3)
				Y_ENA_RESET;
			break;
		case 0:
			if(MOTOx == TIM3)
				Y_ENA_SET;
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
        __ARR[i] = (uint16_t)((T3_FREQ/fre[i]) -  1);
    }
	
    return;
}




/********************************************************
 * step:  移动的步数，正为顺时针，负为逆时针
 * speed: 最大角速度(rad/s)
 * 800脉冲一圈
 * ******************************************************/
void Y_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible)
{
    CalculateSModelLine(__FRE, __ARR, Y_STEP_S, fre_max, fre_min, flexible);

    step = step - Y_pos;         // 绝对位移
	
    Y_ENA(Y_TIMx, ENABLE);

    // 设置运动方向
    if(step < 0)
    {
        srd_y.dir = CCW;
		Y_DIR_RESET;
        
		step = -step; 
    }else{
        srd_y.dir = CW;
		Y_DIR_SET;                   // step为正，ENA为+，顺时针
    }
	
	srd_y.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
    if(step == 1)
    {
        srd_y.accel_count = 1;
        srd_y.step_arr = (int16_t)((T3_FREQ/fre_min) - 1);
        srd_y.run_state = DECEL;
        Y_Status = 1;
    }else if(step != 0){
        if(step <= (Y_STEP_S*2))
        {
            srd_y.step_arr = __ARR[0];
            srd_y.accel_count = (int32_t)(step/2);
            srd_y.decel_start = srd_y.accel_count;
            srd_y.run_state = ACCEL;
            Y_Status = 1;
			
        }else{
            srd_y.step_arr = __ARR[0];
            srd_y.accel_count = Y_STEP_S;
            srd_y.decel_start = step - Y_STEP_S;
            srd_y.run_state = ACCEL;
            Y_Status = 1;
        }
    }
	
    Y_TIM_SetAutoreload(Y_TIMx, srd_y.step_arr);
    Y_TIM_SetAutoreload(Y_TIMx, (srd_y.step_arr/2));

    TIM_Cmd(Y_TIMx, ENABLE);                    // 定时器使能
}


/***************************************
 * 
 *           TIM2中断处理函数
 * 
 * ************************************/
void TIM3_IRQHandler(void)
{
	
    static uint16_t step_count = 0;                 // 总移步数计数器

	//printf("step_count:%d\r\n", step_count);
	
    if(TIM_GetITStatus(Y_TIMx, TIM_IT_Update) != RESET)
    {
        switch(srd_y.run_state)
        {
            case STOP:
                step_count = 0;

                TIM_Cmd(Y_TIMx, DISABLE);
                Y_ENA_RESET;
			
                Y_Status = 0;

                break;

            case ACCEL:
                step_count++;
			
                if(srd_y.dir==CW)
                {
                    Y_pos++;
                }else{
                    Y_pos--;
                }

                if(step_count < srd_y.accel_count)
                {
                    srd_y.step_arr = __ARR[step_count];
                    srd_y.run_state = ACCEL;
					
                }else if(step_count >= srd_y.decel_start){                          // 直接进入减速阶段
                    srd_y.step_arr = __ARR[step_count-1];
					
                    srd_y.run_state = DECEL;
					
                }else if(step_count >= Y_STEP_S){    
					// 进入匀速阶段
                    srd_y.step_arr = __ARR[Y_STEP_S-1];
                    srd_y.run_state = RUN;
                }
				
                break;

            case RUN:
                step_count++;
			
                if(srd_y.dir==CW)
                {
                    Y_pos++;
                }else{
                    Y_pos--;
                }

                if(step_count >= srd_y.decel_start)
                {
                    srd_y.step_arr = __ARR[Y_STEP_S-1];
                    srd_y.run_state = DECEL;
                }
                break;

            case DECEL:
                step_count++;
			
                if(srd_y.dir==CW)
                {
                    Y_pos++;
                }else{
                    Y_pos--;
                }
                
                if(step_count < srd_y.total_count)
                {
                    srd_y.step_arr = __ARR[srd_y.total_count-step_count];
                }else{
					srd_y.run_state = STOP;
                }
                break;

        }
        Y_TIM_SetAutoreload(Y_TIMx, srd_y.step_arr);
        Y_TIM_SetCompare2(Y_TIMx, (srd_y.step_arr/2));

        TIM_ClearITPendingBit(Y_TIMx, TIM_IT_Update);
    }
}



