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

 int encoder_val=0; 
 #define MIDDLE  32767 
 
 void Timer_Encoder_Setup() 
 { 
    GPIO_InitTypeDef GPIO_InitStructure; 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
         TIM_OCInitTypeDef TIM_OCInitStructure; 
         /*Set RCC for Timer 2*/ 
         RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
         /*Timer Base Initialization for timer2  */ 
         TIM_TimeBaseInitStructure.TIM_Prescaler=0x00; 
         TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up ; 
         TIM_TimeBaseInitStructure.TIM_Period = 65535; 
         TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1; 
         TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure); 
  
         // Configures the encoder mode TI12 for TIM2 
         TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, 
        TIM_ICPolarity_Rising); 
         /*Start timer 2 */ 
         TIM_Cmd(TIM2, ENABLE); 
         /*set MIDDLE value*/ 
         TIM_SetCounter(TIM2, MIDDLE); 
         /*Set RCC for GPIOA */ 
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
         /*------------------ GPIO for Encoder --------------------*/ 
         /* PA0 : Timer2 Channel1 for Encoder phase A              */ 
         /* PA1 : Timer2 Channel2 for Encoder phase B              */ 
         GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0  |  GPIO_Pin_1  ; 
         GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; 
         GPIO_Init(GPIOA, &GPIO_InitStructure); 
 } 
 void Encoder_Update(void) 
 { 
      unsigned int ReadEncoder = 0; 
      int dummy = 0; 
      int Last_encoder =0; 
      Last_encoder = encoder_val; 
      // For Encoder Update 
      ReadEncoder = TIM_GetCounter(TIM2); 
    TIM_SetCounter(TIM2, MIDDLE); 
      dummy = ReadEncoder - MIDDLE; 
      encoder_val = Last_encoder + dummy; 
      // one round is 300 pulses but the timer multiply it by 4 // 
      // therefore one round is 300x4 = 1200 pulses/round // 
 }

 void DelayMC(unsigned int ms)
 {
 volatile unsigned int nCount;
 nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
 while(nCount--);
 }

static void __reverse(char* start, char* end)
{
  char tmp;
  while (end > start)
  {
    tmp = *end;
    *end-- = *start;
    *start++ = tmp;
  }
}

char* itoa(int value, char* buffer, int base)
{
  static const char digits [] = "0123456789ABCDEF";
  char* tmpBuffer = buffer;
  int sign = 0;
  int quot, rem;
  if ((sign = value) < 0) /* record sign */
    value = -value; /* make value positive */

  if ( (base >= 2) && (base <= 16) )
  {
   do
   { quot = value / base;
     rem = value % base;
     *buffer ++ = digits[rem];
   } while ((value = quot));

   if (sign < 0)
     *buffer ++ = '-';
    __reverse(tmpBuffer, buffer - 1);
    }
 *buffer = '\0';
 return tmpBuffer;
}

void USART_setup()
 {
    USART_InitTypeDef usart_init_struct;
    /* Baud rate 115200, 8-bit data, One stop bit
    * No parity, Do both Rx and Tx, No HW flow control*/
    usart_init_struct.USART_BaudRate = 115200;
    usart_init_struct.USART_WordLength = USART_WordLength_8b;
    usart_init_struct.USART_StopBits = USART_StopBits_1;
    usart_init_struct.USART_Parity = USART_Parity_No ;
    usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1*/
    USART_Init(USART1, &usart_init_struct);
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
 }   

void USART1_sendC(unsigned char c)
 {   
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
    USART_SendData(USART1,(unsigned char) c);
 }

void USART1_putS(unsigned char *s)
 {
    while (*s)
    {
        USART1_sendC(*s);
        s++; 
    }
 }

 int main(void) 
 { 
     unsigned int state = 0; 
     unsigned char sbuf[20]; 
     GPIO_Configuration(); 
     GPIO_WriteBit(GPIOC,GPIO_Pin_13,0); 
     Timer_Encoder_Setup();  
     USART_setup(); 
     USART1_putS("Reading Encoder: \n\r\0"); 
     while(1) 
     { 
      if (state == 0) { 
            GPIO_WriteBit(GPIOC,GPIO_Pin_13,1); 
            state = 1; 
      } 
      if (state == 1) { 
            GPIO_WriteBit(GPIOC,GPIO_Pin_13,0); 
            state = 0; 
      } 
      Encoder_Update(); 
      itoa(encoder_val,sbuf,10); 
      USART1_putS(sbuf); 
      USART1_putS("\n\r"); 
      DelayMC(100); 
     } 
 }
  
