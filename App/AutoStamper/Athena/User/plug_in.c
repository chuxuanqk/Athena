#include "plug_in.h"



Command cmd;
float FRE_MAX = 12800.0;
float FRE_MIN = 800.0;
float FLEXIBLE = 5.0;


int Power(int base, int32_t exponent)
{
    int ret=1.0;
    int i = 0;
    for(; i<=exponent; ++i)
    {
        ret*=base;
    }

    return ret;
}


/**********************************
 * 
 *      计算usart发送的信息size
 * 
 * *******************************/
int32_t Usart_Size(u8 *buf)
{
    int count=0;

    while(*buf != 0x0d)
    {
        ++buf;
        ++count;
    }
    return count;
}


/****************************************
 * 
 *       串口接收值转为INT
 * 
 **************************************/
int32_t Recv_Int(void)
{
    int32_t Value=0;
    uint16_t i=0;
    int32_t temp;

    if(USART_RX_STA&0x8000)
    {
        for(; i<(USART_RX_STA&0x3FFF); ++i)
        {   
			printf("Buf:%d\r\n", USART_RX_BUF[i]);
            temp = (USART_RX_BUF[i] - 48)*Power(10, i);
            Value += temp;
        }
		USART_RX_STA=0;
    }
    
    return Value;
}

/****************************************
 * 
 *       解析串口传输的命令
 * 
 **************************************/
int16_t Decode(void)
{
    uint16_t i=0;

    if(USART_RX_STA&0x8000)
    {
        if((USART_RX_STA&0x3FFF) != 12){
            return -1;
        }else{
            cmd.X_STEP =  (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
            cmd.Y_STEP =  (USART_RX_BUF[3]-48)*100+(USART_RX_BUF[4]-48)*10+(USART_RX_BUF[5]-48);
            cmd.Z_STEP =  (USART_RX_BUF[6]-48)*100+(USART_RX_BUF[7]-48)*10+(USART_RX_BUF[8]-48);
            cmd.state  =  USART_RX_BUF[9];
            cmd.flag   =  USART_RX_BUF[10];
        }
        USART_RX_STA=0;
    }
    return 0;
}



/*************************************
 * 
 *          盖章系统初始化
 * 
 * ***********************************/
void Stamper_Init(void)
{
    delay_init();
	NVIC_Configuration();

    uart_init(9600);
    LED_Init();
	EXTIX_Init();

    TIM2_CH2_Init();             
    TIM3_CH2_Init();
    TIM4_CH3_Init();
    Catch_GPIO_Init();
}


/****************************************
 * 
 *          X和Y轴运动状态
 * 
 * **************************************/
void X_Y_Move(int32_t x_step, int32_t y_step)
{
    X_MoveAbs(x_step, FRE_MAX, FRE_MIN, FLEXIBLE);
    Y_MoveAbs(y_step, FRE_MAX, FRE_MIN, FLEXIBLE);
}


/****************************************
 * 
 *          Z轴和抓手的运动状态
 * 
 ***************************************/
void Z_Catch_Move(int32_t z_step, int8_t state)
{
    Z_MoveAbs(z_step, FRE_MAX, FRE_MIN, FLEXIBLE);

    switch(state)
    {
        case 0:
            Catch(OPEN);
            break;
        
        case 1:
            Catch(CLOSE);
            break;
            
        case 2:
            Catch(OFF);
            break;
        default:
            break;
    }
}


/***********************************
 *
 *        盖章动作测试模块,
 *        第一阶段：移动X，Y轴,后抓取印章/放下印章   
 * 
 * *********************************/
void First_Stage(void)
{
    int32_t Value = 0;

    Decode();

    if(cmd.flag == 1)
    {
        X_Y_Move(cmd.X_STEP, cmd.Y_STEP);
        delay_s(2);
        Z_Catch_Move(cmd.Z_STEP, cmd.state);

        cmd.flag = 0;
    }
}



