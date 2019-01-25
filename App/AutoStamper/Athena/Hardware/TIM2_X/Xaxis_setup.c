/*
This is the program done by David Xue.Based on the 硬石电机控制指导手册
*/


#include "Xaxis_setup.h"
#include "usart.h"


XspeedRampData srd_x;   //手册当中还设置了{stop, cw,0,0,0}
int32_t X_Status = 0; //是否在运动？0：停止，1：运动
int32_t  X_pos   = 0; //当前位置 

int32_t flag = 1;

//开方函数
uint32_t X_sqrt(uint32_t x)
{
  register uint32_t xr;  // result register
  register uint32_t q2;  // scan-bit register
  register uint8_t f;   // flag (one bit)

  xr = 0;                     // clear result
  q2 = 0x40000000L;           // higest possible result bit
  do
  {
    if((xr + q2) <= x)
    {
      x -= xr + q2;
      f = 1;                  // set flag
    }
    else{
      f = 0;                  // clear flag
    }
    xr >>= 1;
    if(f){
      xr += q2;               // test flag
    }
  } while(q2 >>= 2);          // shift twice
  if(xr < x){
    return xr +1;             // add for rounding
  }
  else{
    return xr;
	  }
}




static void X_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//脉冲输出端配置
	RCC_APB2PeriphClockCmd(X_PULSE_CLK|X_DIR_CLK|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = X_PULSE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出，指导书用的是复用开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(X_PULSE_PORT, &GPIO_InitStructure);
	//方向和使能GPIO配置
	GPIO_InitStructure.GPIO_Pin = X_DIR_PIN | X_ENA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// 为什么选择推挽,指导书用开漏
	GPIO_Init(X_DIR_ENA_PORT, &GPIO_InitStructure);

}

static void X_NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);

}

void X_TIM_init(void)//定时器配置
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef   TIM_OCInitStructure;
	
	X_TIM_CLOCKCMD(X_TIM_RCC_CLK, ENABLE);
	
	X_GPIO_init();
	X_NVIC_init();
	
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 8; //预分频后得到的频率是8M，介绍里面设置的是3
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(X_TIMx, &TIM_TimeBaseStructure);
	
	//比较输出配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//手册中用的是反转输出
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1000; //设置CCR的值，当CNT的计数超过CCR时，通道无效，因为使用的是PWM1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	X_TIM_OCnInit(X_TIMx, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_ClearFlag(X_TIMx, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
	
	
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
				GPIO_ResetBits(GPIOC, X_ENA_PIN);
			break;
		case 0:
			if(MOTOx == TIM2)
				GPIO_SetBits(GPIOC,X_ENA_PIN);
			break;
	}
}

/*
功能：相对位置运动，转到给定的步数
输入参数：step: 移动的步数 正为顺时针，负为逆时针
         accel: 加速度 实际值为accel*1 弧度每秒平方
         decel：减速度
				 speed 最大速度
返回值：无
说明：给定步数，电机先加速到最大速度，然后在某个位置开始减速至停止，整个过程的步数
      为指定步数。如果加减速很短并且速度很小，还没有达到最大速度就减速
*/
void X_MoveAbs(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
	uint32_t max_s_lim;//达到最大速度需要的步数
	uint32_t accel_lim;//减速开始之前的步数
	
	//float ftemp=0;
	
	step=step-X_pos;
	
	X_ENA(TIM2,ENABLE);
	
  //当步数为负数或正数时，对方向口设置高低电平
	if(step < 0)
	{
		srd_x.dir = CCW;
		GPIO_ResetBits(X_DIR_ENA_PORT, X_DIR_PIN);
		step =-step;
	}
	else
	{
		srd_x.dir = CW;
		GPIO_SetBits(X_DIR_ENA_PORT, X_DIR_PIN);
	}
	
 //当step给1时
	if(step == 1)
	{
		srd_x.accel_count = -1;
		srd_x.run_state =DECEL;
		srd_x.step_delay = 1000;
		X_Status = 1;
	}
	else if(step != 0)//如果目标运动步数不为0
	{
	  //设置最大速度极限
		srd_x.min_delay = (int32_t)(A_T_x10/speed);
		//计算第一个C0的step_delay 来设定加速度
		//srd_x.step_delay = (int32_t)(T1_FREQ_148*X_sqrt(A_SQ*10000/accel)/100);
		srd_x.step_delay = (int32_t)((T1_FREQ*0.676* X_sqrt(ALPHA*10000/accel)/100));
		
		printf("T1:%d", T1_FREQ);
		
		printf("srd_x.step_delay:%d\n\r", srd_x.step_delay);
		
	  //计算多少步之后达到最大速度限制(加速的值需要除以10才是真实值)
		max_s_lim = (uint32_t)(speed*speed/(2*ALPHA*accel));
		
		
		//如果最大速度小于0.5步，近似为0
		if(max_s_lim == 0){		
			max_s_lim = 1;
		}
		//计算多少步之后开始减速
		accel_lim = (uint32_t)(step*decel/(accel+decel));
		
		if(accel_lim == 0){
		accel_lim =1;		
		}
		
		//当加速阶段未达到最大速度就开始减速
		//计算srd_x.decel_val的值
		if(accel_lim <= max_s_lim)
		{
			srd_x.decel_val = accel_lim - step;
		
		}
		else 
		{
			srd_x.decel_val = -(max_s_lim*accel/decel);
		}
		
		//当剩下一步时，必须减速
		if(srd_x.decel_val ==0)
		{	
			srd_x.decel_val = -1;		
		}
		
		
		//计算开始减速时已经走的步数
		srd_x.decel_start = step + srd_x.decel_val;
		
		
		
		//如果最大速度很慢，就不需要进行加速运动，否则使状态变为加速
		if(srd_x.step_delay <= srd_x.min_delay)
		{
			srd_x.step_delay =srd_x.min_delay;
			srd_x.run_state = RUN;
		}
		else
		{
			//srd_x.accel_count = ACCEL;
			srd_x.run_state = ACCEL;
		
		}
		//加速度计数值复位
		srd_x.accel_count = 0;
	}

	//电机为运动状态
	X_Status = 1;
	X_TIM_SetCompare2(X_TIMx, srd_x.step_delay/2);//这个什么意思
	TIM_SetAutoreload(X_TIMx, srd_x.step_delay);//这个是什么意思
	
//	printf("accel:%d\n\r", srd_x.accel_count);
	printf("step:%d\n\r", srd_x.step_delay);
//	printf("decel:%d\n\r", srd_x.decel_start);
//	printf("减速:%d\n\r", srd_x.decel_val);
	printf("最大速度:%d\n\r", srd_x.min_delay);
	
	
	TIM_Cmd(X_TIMx, ENABLE);
}
/*
定时器TIM2中断服务函数
功能：实现加减速控制
*/
void TIM2_IRQHandler(void)
{
	uint16_t new_step_delay;//保存下一个延时周期
	static uint16_t last_accel_delay;//加速过程中最后一次的计数器个数，手册把值赋了0
	static uint32_t step_count = 0;//总移步数计数器
	static int32_t rest =0; //记录 new_step_delay 中的余数
	
	//！！！！下面的函数将switch语句包  放入到了中断实现的条件语句中了
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		X_TIM_SetCompare2(X_TIMx, srd_x.step_delay/2);//这个什么意思
		TIM_SetAutoreload(X_TIMx, srd_x.step_delay);
		
		switch(srd_x.run_state)
		{
/*停止状态*/			
			case STOP:
				step_count = 0;
				rest = 0;	
				TIM_Cmd(X_TIMx, DISABLE);
				GPIO_ResetBits(GPIOC, X_ENA_PIN);
				X_Status = 0;
			
				break;
/*加速状态*/			
			case ACCEL:
				step_count++;
				if(srd_x.dir==CW)
				{
					X_pos++;			
				}
				else
				{
					X_pos--;			
				}
				
				srd_x.accel_count++;
				//每次计算step_delay的余数累加到之后的运算, 新周期
				new_step_delay = srd_x.step_delay - (((2*(long)srd_x.step_delay)+rest)/(4*srd_x.accel_count+1));
			 
				rest = ((2*(long)srd_x.step_delay)+rest)%(4*srd_x.accel_count+1);
				
				if(step_count>= srd_x.decel_start)
				{
				srd_x.accel_count = srd_x.decel_val;//不清楚什么意思，为什么不把accel_count的负值设置给decel_val
				srd_x.run_state = DECEL;			
				}
				else if(new_step_delay <= srd_x.min_delay)//这个判断语句是在说如果某个时刻的速度小于等于最高速度
				{
				last_accel_delay = new_step_delay;//新的速度量设定给最后
				new_step_delay = srd_x.min_delay;//把最高速度 给了最后的加速状态
				rest = 0;
				srd_x.run_state = RUN;
				}
				break;
			/*匀速状态运行*/			
			case RUN:
				step_count++;
				if(srd_x.dir == CW)
				{
					X_pos++;
				
				}
				else
				{
					X_pos--;
				}
				new_step_delay = srd_x.min_delay;//把最高速度设置给新的速度量
				if(step_count >= srd_x.decel_start)
				{
					srd_x.accel_count= srd_x.decel_val;
					new_step_delay = last_accel_delay;
					srd_x.run_state = DECEL;
				}
				break;
/*减速运动*/				
			case DECEL:
				step_count++;
				if(srd_x.dir == CW)
				{
					X_pos++;
				}
				else
				{
					X_pos--;
				}
				srd_x.accel_count++;
				new_step_delay = srd_x.step_delay - (((2*(long)srd_x.step_delay)+rest)/(4*srd_x.accel_count+1));
				rest = ((2*(long)srd_x.step_delay)+rest)%(4*srd_x.accel_count+1);
				
				if(srd_x.accel_count >= 0)
				{
					srd_x.run_state = STOP;
				}
				break;	
		}
		srd_x.step_delay = new_step_delay;

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}

}




