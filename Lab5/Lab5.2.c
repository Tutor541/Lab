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

void Timer3Setup() 
 { 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
    TIM_OCInitTypeDef TIM_OCInitStructure; 
    /*    if 72MHz clock is used 
    timer_tick_frequency = Timer_default_frequency / (prescaller + 1) 
    timer_tick_frequency = 72,000,000 / (143 + 1) = 72,000,000/144 = 500,000 Hz 
    500,000 Hz -> 0.5 MHz -> T = 2 usec 
    Timer count 50,000 time -> Timer period -> 100,000 usec = 100 msec 
    PWM has 50% duty cycle from TIM_Pulse = 50% 0f Timer count period    */ 
    /*Set RCC for Timer 3*/ 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
    /*Timer Base Initialization for timer3  */ 
    TIM_TimeBaseInitStructure.TIM_Prescaler =0x8F; // 0x47 = (72 x2) -1 =143  // 
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up ; 
    TIM_TimeBaseInitStructure.TIM_Period = 50000 - 1; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1; 
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure); 
    /* Timer3 Output Compare Setup */ 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable ; 
    TIM_OCInitStructure.TIM_Pulse = 25000 -1 ; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OC4Init(TIM3, &TIM_OCInitStructure); ///PB1 - Output PWM on channel 4 
    /* Enable Timer3 */ 
    TIM_Cmd(TIM3,ENABLE); 
 } 

void PWM_Set(unsigned int Input) 
 { 
    unsigned int Pulse = 0; 
    TIM_OCInitTypeDef TIM_OCInitStructure; 
    // 4095 = 50000 then Pulse = Input ADC *50000/4095 
    Pulse = (unsigned int) (Input*50000/4095); 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable ; 
    TIM_OCInitStructure.TIM_Pulse = Pulse-1; 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
    TIM_OC4Init(TIM3, &TIM_OCInitStructure); 
 } 

 int main(void) 
 { 
    unsigned int ADCRead_value = 0; 
    GPIO_Configuration(); 
    GPIO_WriteBit(GPIOC,GPIO_Pin_13,0); 
    Timer3Setup(); 
    ADC1_Init();
    while(1)
    {
        ADCRead_value = readADC1(0);
        PWM_Set(ADCRead_value);
        DelayMC(100);
    }
 }
  
