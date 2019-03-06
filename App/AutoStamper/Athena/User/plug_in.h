#ifndef __PLUG_IN_H
#define __PLUG_IN_H

#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "X_Setup.h"
#include "Y_Setup.h"
#include "Z_Setup.h"
#include "catch.h"

int Power(int base, int32_t exponent);
int32_t Usart_Size(u8 *buf);
int32_t Recv_Int(void);
int16_t Decode(void);


void Stamper_Init(void);
void X_Y_Move(int32_t x_step, int32_t y_step);
void Z_Catch_Move(int32_t z_step, int8_t state);

void First_Stage(void);

extern float FRE_MAX;
extern float FRE_MIN;
extern float FLEXIBLE;

typedef struct Command{
    int16_t X_STEP;
    int16_t Y_STEP;
    int16_t Z_STEP;
    int16_t state;
    int16_t flag;
}Command; 

#endif
