/*
This is the program done by David Xue.Based on the Ӳʯ�������ָ���ֲ�
*/


#include "Xaxis_setup.h"
#include "usart.h"


XspeedRampData srd_x;   //�ֲᵱ�л�������{stop, cw,0,0,0}
int32_t X_Status = 0; //�Ƿ����˶���0��ֹͣ��1���˶�
int32_t  X_pos   = 0; //��ǰλ�� 

int32_t flag = 1;

//��������
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
	//�������������
	RCC_APB2PeriphClockCmd(X_PULSE_CLK|X_DIR_CLK|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = X_PULSE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�������������ָ�����õ��Ǹ��ÿ�©
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(X_PULSE_PORT, &GPIO_InitStructure);
	//�����ʹ��GPIO����
	GPIO_InitStructure.GPIO_Pin = X_DIR_PIN | X_ENA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// Ϊʲôѡ������,ָ�����ÿ�©
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

void X_TIM_init(void)//��ʱ������
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef   TIM_OCInitStructure;
	
	X_TIM_CLOCKCMD(X_TIM_RCC_CLK, ENABLE);
	
	X_GPIO_init();
	X_NVIC_init();
	
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 8; //Ԥ��Ƶ��õ���Ƶ����8M�������������õ���3
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(X_TIMx, &TIM_TimeBaseStructure);
	
	//�Ƚ��������
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//�ֲ����õ��Ƿ�ת���
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1000; //����CCR��ֵ����CNT�ļ�������CCRʱ��ͨ����Ч����Ϊʹ�õ���PWM1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	X_TIM_OCnInit(X_TIMx, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_ClearFlag(X_TIMx, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
	
	
}
/*
�������ܣ����ʹ��
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
���ܣ����λ���˶���ת�������Ĳ���
���������step: �ƶ��Ĳ��� ��Ϊ˳ʱ�룬��Ϊ��ʱ��
         accel: ���ٶ� ʵ��ֵΪaccel*1 ����ÿ��ƽ��
         decel�����ٶ�
				 speed ����ٶ�
����ֵ����
˵������������������ȼ��ٵ�����ٶȣ�Ȼ����ĳ��λ�ÿ�ʼ������ֹͣ���������̵Ĳ���
      Ϊָ������������Ӽ��ٺ̲ܶ����ٶȺ�С����û�дﵽ����ٶȾͼ���
*/
void X_MoveAbs(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
	uint32_t max_s_lim;//�ﵽ����ٶ���Ҫ�Ĳ���
	uint32_t accel_lim;//���ٿ�ʼ֮ǰ�Ĳ���
	
	//float ftemp=0;
	
	step=step-X_pos;
	
	X_ENA(TIM2,ENABLE);
	
  //������Ϊ����������ʱ���Է�������øߵ͵�ƽ
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
	
 //��step��1ʱ
	if(step == 1)
	{
		srd_x.accel_count = -1;
		srd_x.run_state =DECEL;
		srd_x.step_delay = 1000;
		X_Status = 1;
	}
	else if(step != 0)//���Ŀ���˶�������Ϊ0
	{
	  //��������ٶȼ���
		srd_x.min_delay = (int32_t)(A_T_x10/speed);
		//�����һ��C0��step_delay ���趨���ٶ�
		//srd_x.step_delay = (int32_t)(T1_FREQ_148*X_sqrt(A_SQ*10000/accel)/100);
		srd_x.step_delay = (int32_t)((T1_FREQ*0.676* X_sqrt(ALPHA*10000/accel)/100));
		
		printf("T1:%d", T1_FREQ);
		
		printf("srd_x.step_delay:%d\n\r", srd_x.step_delay);
		
	  //������ٲ�֮��ﵽ����ٶ�����(���ٵ�ֵ��Ҫ����10������ʵֵ)
		max_s_lim = (uint32_t)(speed*speed/(2*ALPHA*accel));
		
		
		//�������ٶ�С��0.5��������Ϊ0
		if(max_s_lim == 0){		
			max_s_lim = 1;
		}
		//������ٲ�֮��ʼ����
		accel_lim = (uint32_t)(step*decel/(accel+decel));
		
		if(accel_lim == 0){
		accel_lim =1;		
		}
		
		//�����ٽ׶�δ�ﵽ����ٶȾͿ�ʼ����
		//����srd_x.decel_val��ֵ
		if(accel_lim <= max_s_lim)
		{
			srd_x.decel_val = accel_lim - step;
		
		}
		else 
		{
			srd_x.decel_val = -(max_s_lim*accel/decel);
		}
		
		//��ʣ��һ��ʱ���������
		if(srd_x.decel_val ==0)
		{	
			srd_x.decel_val = -1;		
		}
		
		
		//���㿪ʼ����ʱ�Ѿ��ߵĲ���
		srd_x.decel_start = step + srd_x.decel_val;
		
		
		
		//�������ٶȺ������Ͳ���Ҫ���м����˶�������ʹ״̬��Ϊ����
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
		//���ٶȼ���ֵ��λ
		srd_x.accel_count = 0;
	}

	//���Ϊ�˶�״̬
	X_Status = 1;
	X_TIM_SetCompare2(X_TIMx, srd_x.step_delay/2);//���ʲô��˼
	TIM_SetAutoreload(X_TIMx, srd_x.step_delay);//�����ʲô��˼
	
//	printf("accel:%d\n\r", srd_x.accel_count);
	printf("step:%d\n\r", srd_x.step_delay);
//	printf("decel:%d\n\r", srd_x.decel_start);
//	printf("����:%d\n\r", srd_x.decel_val);
	printf("����ٶ�:%d\n\r", srd_x.min_delay);
	
	
	TIM_Cmd(X_TIMx, ENABLE);
}
/*
��ʱ��TIM2�жϷ�����
���ܣ�ʵ�ּӼ��ٿ���
*/
void TIM2_IRQHandler(void)
{
	uint16_t new_step_delay;//������һ����ʱ����
	static uint16_t last_accel_delay;//���ٹ��������һ�εļ������������ֲ��ֵ����0
	static uint32_t step_count = 0;//���Ʋ���������
	static int32_t rest =0; //��¼ new_step_delay �е�����
	
	//������������ĺ�����switch����  ���뵽���ж�ʵ�ֵ������������
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		X_TIM_SetCompare2(X_TIMx, srd_x.step_delay/2);//���ʲô��˼
		TIM_SetAutoreload(X_TIMx, srd_x.step_delay);
		
		switch(srd_x.run_state)
		{
/*ֹͣ״̬*/			
			case STOP:
				step_count = 0;
				rest = 0;	
				TIM_Cmd(X_TIMx, DISABLE);
				GPIO_ResetBits(GPIOC, X_ENA_PIN);
				X_Status = 0;
			
				break;
/*����״̬*/			
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
				//ÿ�μ���step_delay�������ۼӵ�֮�������, ������
				new_step_delay = srd_x.step_delay - (((2*(long)srd_x.step_delay)+rest)/(4*srd_x.accel_count+1));
			 
				rest = ((2*(long)srd_x.step_delay)+rest)%(4*srd_x.accel_count+1);
				
				if(step_count>= srd_x.decel_start)
				{
				srd_x.accel_count = srd_x.decel_val;//�����ʲô��˼��Ϊʲô����accel_count�ĸ�ֵ���ø�decel_val
				srd_x.run_state = DECEL;			
				}
				else if(new_step_delay <= srd_x.min_delay)//����ж��������˵���ĳ��ʱ�̵��ٶ�С�ڵ�������ٶ�
				{
				last_accel_delay = new_step_delay;//�µ��ٶ����趨�����
				new_step_delay = srd_x.min_delay;//������ٶ� �������ļ���״̬
				rest = 0;
				srd_x.run_state = RUN;
				}
				break;
			/*����״̬����*/			
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
				new_step_delay = srd_x.min_delay;//������ٶ����ø��µ��ٶ���
				if(step_count >= srd_x.decel_start)
				{
					srd_x.accel_count= srd_x.decel_val;
					new_step_delay = last_accel_delay;
					srd_x.run_state = DECEL;
				}
				break;
/*�����˶�*/				
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




