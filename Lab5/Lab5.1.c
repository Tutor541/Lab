#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

 void timer3_delay() 
 { 
      TIM_Cmd(TIM3,ENABLE); 
      // wait until timer 3 done counting // 
      while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) != SET); 
      TIM_ClearFlag(TIM3,TIM_FLAG_Update); 
      TIM_Cmd(TIM3,DISABLE); 
 } 

 void GPIO_Configuration() 
 { 
    GPIO_InitTypeDef GPIO_InitStruct; 
    // Enable clock for GPIOB and GPIOC and GPIOA and AFIO  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE); 
    // Configure PA0 and PA1 as Analog input 
    GPIO_InitStruct.GPIO_Pin= GPIO_Pin_0 | GPIO_Pin_1; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN; 
    GPIO_Init(GPIOA, &GPIO_InitStruct); 
    // Configure PC13 as output push pull 50MHz 
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13; 
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOC, &GPIO_InitStruct); 
     // Configure PB15 as output push pull 50MHz 
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15; 
    GPIO_Init(GPIOB, &GPIO_InitStruct); 
    // Configure PB1 as PWM output Timer 3 OC 4 
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStruct); 
 } 

 . void Timer3Setup() 
 { 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
    /* 
    TIM3 is connected to APB1 bus, 
    if 72MHz clock is used 
    Timer count frequency is set with 
    timer_tick_frequency = Timer_default_frequency / (prescaller + 1) 
    timer_tick_frequency = 72,000,000 / (71 + 1) = 72,000,000/72 = 1,000,000 Hz 
    1,000,000 Hz -> 1 MHz -> T = 1 usec 
    Timer count 50,000 time -> Timer period -> 50,000 usec = 50 msec 
    */ 
    /*Set RCC for Timer 3*/ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
    /*Timer Base Initialization for timer3  */ 
    TIM_TimeBaseInitStructure.TIM_Prescaler=0x47; // 0x47 = 71 // 
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up ; 
    TIM_TimeBaseInitStructure.TIM_Period = 49999; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1; 
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure); 
 } 

 int main(void)
 {
     unsigned char state = 0;
     GPIO_Configuration();
     GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
     Timer3Setup();
     while(1)
     {
     if (state == 0) {
     GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
     state = 1; }
     else {
     GPIO_WriteBit(GPIOB,GPIO_Pin_15,0);
     state = 0; }
     timer3_delay();
     }   
}