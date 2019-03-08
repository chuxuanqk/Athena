#include "X_Setup.h"
#include "usart.h"

XspeedRampData srd_x; 
int32_t X_Status = 0;           // 是否在运动？ 0：停止， 1：运动
uint32_t X_pos = 0;               // 当前位置


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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);        // 使能TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // 使能GPIOA

	DIR_ENA_Init();									// 初始化DIR_ENA

    TIM2_RemapConfig_Init();                        // 配置重映射
    GPIOA_1_CH2_Init();                             // 初始化GPIOA_1
    TIM2_NVIC_Init();                               // 初始化TIM2中断配置
    TIM2_Init(9999, 8);                             // 初始化TIM2
    TIM2_PWM_Init();                                // 配置TIM2的PWM模式
    
    TIM_OC2PreloadConfig(X_TIMx, TIM_OCPreload_Enable);             // 使能重装载
    TIM_ClearFlag(X_TIMx, TIM_FLAG_Update);                         // 设置FLAG更新
    TIM_ITConfig(X_TIMx, TIM_IT_Update, ENABLE);                    // 使能通用定时器中断

    //TIM_SetAutoreload(TIM2, 19999);               // 设置自动重装载值
	//TIM_SetCompare1(TIM2, 9999);               // 设置比较值

	TIM_Cmd(X_TIMx, DISABLE);
    //TIM_Cmd(X_TIMx, ENABLE);                      // 使能TIM2
}


/********************************************
*    函数功能：驱动器方向、使能端口初始化
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
*           函数功能：电机使能
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
        x = flexible*(i-(len/2))/(len/2);                 //归一化
        y = 1.0/(1+exp(-x));
		
		
        __FRE[i] = D_value*y + fre_min;
        __ARR[i] = (uint16_t)((T1_FREQ/fre[i]) -  1);
    }
	
	
    return;
}

/********************************************************
 * step:  移动的步数，正为顺时针，负为逆时针
 * speed: 最大角速度(rad/s)
 * 800脉冲一圈
 * ******************************************************/
void X_MoveAbs(int32_t step, float fre_max, float fre_min, float flexible)
{
    CalculateSModelLine(__FRE, __ARR, X_STEP_S, fre_max, fre_min, flexible);
  
    step = step - X_pos;         // 绝对位移
	
    X_ENA(X_TIMx, ENABLE);

    // 设置运动方向
    if(step < 0)
    {
        srd_x.dir = CCW;
		X_DIR_RESET;
        
		step = -step; 
    }else{
		//printf("step>0\r\n");
        srd_x.dir = CW;
		X_DIR_SET;                   // step为正，ENA为+，顺时针
    }

	srd_x.total_count = step;    // 记录要走的总步数

    // 当旋转角度很小时，直接以启动频率开始运行
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

    TIM_Cmd(X_TIMx, ENABLE);                    // 定时器使能
}


/***************************************
 * 
 *           TIM2中断处理函数
 * 
 * ************************************/
void TIM2_IRQHandler(void)
{
    static uint16_t step_count = 0;                 // 总移步数计数器

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
					
                }else if(step_count >= srd_x.decel_start){                          // 直接进入减速阶段
                    srd_x.step_arr = __ARR[step_count-1];
                    srd_x.run_state = DECEL;
					
                }else if(step_count >= X_STEP_S){    
					// 进入匀速阶段
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



