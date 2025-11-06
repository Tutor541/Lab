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

int main(void)
{
 unsigned char state = 0;
 GPIO_Configuration();
 GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
 USART_setup();
 NVIC_Configuration();
 EXIT_init();
 while(1)
 {
    if (state == 0) {
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
        state = 1; }
    else {
        GPIO_WriteBit(GPIOB,GPIO_Pin_15,0);
        state = 0; }
    DelayMC(100);
    }
 }