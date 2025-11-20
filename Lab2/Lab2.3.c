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

unsigned char USART1_getC()
{
 while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
 return(USART_ReceiveData(USART1));
}

unsigned int toi(unsigned char c)
{
 unsigned int num=-1;
 if(c=='0') num=0;
 if(c=='1') num=1;
 if(c=='2') num=2;
 if(c=='3') num=3;
 if(c=='4') num=4;
 if(c=='5') num=5;
 if(c=='6') num=6;
 if(c=='7') num=7;
 if(c=='8') num=8;
 if(c=='9') num=9;
 return(num);
}

void USART1_putS(unsigned char *s)
{
 while (*s)
 {
    USART1_sendC(*s);
    s++; 
 }
}

void flush_USART1(void)
{
 while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
 // There are some data left in RX
 {
    USART_ReceiveData(USART1); //Flush that data
 }
}

unsigned char CheckQuestion(unsigned char *Ques, unsigned char *In)
{
 unsigned char result = 1, i = 0;
 while(Ques[i]!='\0')
 {
   if (Ques[i] != In[i])
      result = 0;
   i++;
 }
 return(result);
}

int main(void)
{
 unsigned char quest[12]= {'0','0','0','0','0','0','0','0','0','0','0','0'};
 unsigned char *text1, sdata=0;
 unsigned char Q1[] = "Hello\0";
 unsigned char Q2[] = "Your name?\0";
 unsigned char Q3[] = "How old?\0";
 unsigned char Q4[] = "Bye\0";
 unsigned char A1[] = "Hello, too!\r\n\0";
 unsigned char A2[] = "I am Talkative.\r\n\0";
 unsigned char A3[] = "I am 9999 years old.\r\n\0";
 unsigned char A4[] = "Please Do not Go !\r\n\0";
 unsigned char Ax[] = "I do not Understand.\r\n\0";
 unsigned char result_compare = -1, iq=0, found = 0;

 GPIO_Configuration();
 GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
 USART_setup();
 while(1)
 {
   text1 = "Please Ask the Question \n\r\0";
   USART1_putS(text1);
   flush_USART1();
   iq=0;
   while(1)
   {
      sdata = USART1_getC();
      if ((sdata=='\n') || (sdata=='\r'))
         break;
      quest[iq] = sdata;
      iq++;
      if (iq>=12)
         break;
   }
   found = 0;
   USART1_putS("Looking for Answer .... \r\n\0");
   result_compare = CheckQuestion(Q1,quest);
   if(result_compare)
   { USART1_putS(A1);
     found = 1; }
   result_compare = CheckQuestion(Q2,quest);
   if(result_compare)
   { USART1_putS(A2);
     found = 1; }
   result_compare = CheckQuestion(Q3,quest);
   if(result_compare)
   { USART1_putS(A3);
     found = 1; }
   result_compare = CheckQuestion(Q4,quest);
   if(result_compare)
   { USART1_putS(A4);
     found = 1; }
   if(found ==0)
   USART1_putS(Ax);
 }
}


 