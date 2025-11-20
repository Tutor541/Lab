#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void DelayMC(unsigned int ms)
{
 volatile unsigned int nCount;
 nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
 while(nCount--);
}

void GPIO_Configuration()
{
 GPIO_InitTypeDef GPIO_InitStruct;
 // Enable clock for GPIOA and GPIO C and AFIO and USART1
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1,ENABLE);
 // Configure PA9 as alternative output push pull
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
  // Configure PA10 as input floating
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 // Configure PC13 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOC, &GPIO_InitStruct);
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

int main(void)
{
 unsigned char sdata = 0;
 GPIO_Configuration();
 GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
 USART_setup();
 sdata = 33;
 while(1)
 {
    USART1_sendC(sdata);
    sdata++;
    if (sdata >= 127)
    {
        sdata = 33;
        USART1_sendC('\n');
        USART1_sendC('\r');
    }
 }
}



 