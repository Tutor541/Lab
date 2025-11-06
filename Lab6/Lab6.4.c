#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
    
void DelayMC(unsigned int ms)
{
 volatile unsigned int nCount;
 nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
 while(nCount--);
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
    s++; }
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

void GPIO_Configuration()
{
 GPIO_InitTypeDef GPIO_InitStruct;
 // Enable clock for GPIOB and GPIOC and GPIOA and AFIO
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
 // Configure PA0 and PA1 as Analog input
 GPIO_InitStruct.GPIO_Pin= GPIO_Pin_0 | GPIO_Pin_1;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 // Configure PC13 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOC, &GPIO_InitStruct);
 // Configure PB1 and PB15 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_15;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
 // Configure PA9 as alternative output push pull for Tx
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 // Configure PA10 as input floating for Rx
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 GPIO_InitStruct.GPIO_Pin= GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void NVIC_Configuration(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
 // Enable the EXTI3 Interrupt (IRQ vector for Pins Px3)
 NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
 // Enable the EXTI4 Interrupt (IRQ vector for Pins Px4)
 NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
 // Enable the EXTI9_5 Interrupt (IRQ vector for Pins Px8 and Px9)
 NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
}

void EXIT_init(void)
{
 EXTI_InitTypeDef EXTI_InitStructure;
 // Connect EXTI Line to PB3 PB4 PB8 and PB9
 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3) ;
 EXTI_InitStructure.EXTI_Line = EXTI_Line3; //Pin PB3
 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //Rising to Interrupt
 EXTI_InitStructure.EXTI_LineCmd = ENABLE; //Enable Interrupt
 EXTI_Init(&EXTI_InitStructure);

 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
 EXTI_InitStructure.EXTI_Line = EXTI_Line4; // Pin PB4
 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //Rising to Interrupt
 EXTI_InitStructure.EXTI_LineCmd = ENABLE; // Enable Interrupt
 EXTI_Init(&EXTI_InitStructure);

 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
 EXTI_InitStructure.EXTI_Line = EXTI_Line8; // Pin PB8
 EXTI_Init(&EXTI_InitStructure);
 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
 EXTI_InitStructure.EXTI_Line = EXTI_Line9; // Pin PB9
 EXTI_Init(&EXTI_InitStructure);
}

void EXTI3_IRQHandler(void)
{
 USART1_putS("Interrupt occurred ! \n\r");
 if (EXTI_GetITStatus(EXTI_Line3) != RESET)
{
 USART1_putS("Button 3 is pressed \n\r");
 //Clear the EXTI line 3 pending bit
}
 EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
 USART1_putS("Interrupt occurred ! \n\r");
 if (EXTI_GetITStatus(EXTI_Line4) != RESET)
{
 USART1_putS("Button 4 is pressed \n\r");
 //Clear the EXTI line 3 pending bit
}
 EXTI_ClearITPendingBit(EXTI_Line4);
}

void EXTI9_5_IRQHandler(void)
{
 USART1_putS("Interrupt occurred ! \n\r");
 USART1_putS("But which switch is pressed 1 or 2 ??\n\r");
 EXTI_ClearITPendingBit(EXTI_Line8);
 EXTI_ClearITPendingBit(EXTI_Line9);
}

void ADC_Interrupt_Init(void)
{
 USART1_putS("ADC_Interrupt_Init \n\r");
 ADC_InitTypeDef ADC_InitStructure;
 /* PCLK2 is the APB2 clock */ /* ADCCLK = PCLK2/8 = 72/8 = 9MHz*/
 RCC_ADCCLKConfig(RCC_PCLK2_Div8);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 ADC_DeInit(ADC1);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
 /* Disabple the scan conversion do one at a time */
 ADC_InitStructure.ADC_ScanConvMode = DISABLE;
 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
 /* Start conversion by software, not an external trigger */
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
 ADC_InitStructure.ADC_NbrOfChannel = 1;
 ADC_Init(ADC1, &ADC_InitStructure);
 /* Enable Interrupt Event EOC*/
 ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
 /* Enable ADC1 */
 ADC_Cmd(ADC1, ENABLE);
 /* Read from Channel 0 of ADC1 */
 ADC_RegularChannelConfig(ADC1, 0, 1, ADC_SampleTime_55Cycles5);
}

void ADC_NVIC_Config(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
 // Configure interrupt priority
 NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
}

unsigned int ADC_interrupt_value=0;

 void ADC1_2_IRQHandler()
 {
    if (ADC_GetITStatus(ADC1,ADC_IT_EOC) !=RESET)
    {
    // Read ADC conversion value
        ADC_interrupt_value = ADC_GetConversionValue(ADC1);
    }
    ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
}

 void TIM3_Interrupt_Setup()
 {
 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
 TIM_OCInitTypeDef TIM_OCInitStructure;
 /* TIM3 is connected to APB1 bus,
 timer_tick_frequency = 72,000,000 / (71 + 1) = 72,000,000/71 = 1,000,000 Hz
 ,000,000 Hz -> 1 MHz -> T = 1 usec
 times 1 usec = 1000 use = 1 msec */
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
 /*Timer Base Initialization for timer3 */
 TIM_TimeBaseInitStructure.TIM_Prescaler=0x47; // 0x47 = 72 -1 = 71 //
 TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
 TIM_TimeBaseInitStructure.TIM_Period = 1000-1; // 1 msec
 TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
 TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
 TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
}

void TIM3_NVIC_Config(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
 // Priority grouping
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
 // Configure interrupt priority
 NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
}

unsigned int time_msec =0;
unsigned char nstate = 0;
 
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    time_msec++;
    if (nstate == 0) {
      nstate = 1;
      GPIO_WriteBit(GPIOB,GPIO_Pin_15,0);
    }
    else {
        nstate = 0;
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
    }
 }
 TIM_ClearITPendingBit (TIM3,TIM_IT_Update);
}

void DelayTimer(unsigned int msec)
{
 unsigned int current_time = 0;
 current_time = time_msec;
 while( (current_time+msec) >= time_msec);
}

void USART1_NVIC_Config(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
 // Priority grouping
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
 // Configure interrupt priority
 NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
}

unsigned char nstate2 = 0;
void USART1_IRQHandler(void)
{
  unsigned char sData = 0;
  /* RXNE handler */
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    sData = USART_ReceiveData(USART1);
    if(sData == 't')
  {
      if (nstate == 0) {
        nstate = 1;
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
      }
      else {
          nstate = 0;
          GPIO_WriteBit(GPIOB,GPIO_Pin_15,0);
      }
      USART1_putS(" { Done! } \n\r");
    }
    if(sData == 'r')
    {
      if (nstate2 == 0) {
            nstate2 = 1;
            GPIO_WriteBit(GPIOB,GPIO_Pin_1,1);
      }
      else {
            nstate2 = 0;
            GPIO_WriteBit(GPIOB,GPIO_Pin_1,0);
      }
      USART1_putS(" { Done! } \n\r");
  }
  }
}

int main(void)
{
  unsigned char state = 0;
  GPIO_Configuration();
  GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
  USART_setup();
  USART1_NVIC_Config();
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  USART1_putS("<<< Waiting for t and r command >>>\n\r");
  while(1)
  {
    DelayMC(1000);
  }
}

