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

int main(void)
{
 unsigned char sdata1[4]= {'0','0','0','0'}, sdata2[4]= {'0','0','0','0'};
 unsigned char *text1, sdata=0;
 unsigned int number1=0, number2=0, result =0, i1=0, i2=0, temp =0;
 static const char digits [] = "0123456789ABCDEF";
 GPIO_Configuration();
 GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
 USART_setup();
 text1 = "Please type number 1 and enter \n\r\0";
 USART1_putS(text1);
 i1=0;
 while(1)
 {
    sdata = USART1_getC();
    if ((sdata=='\n') || (sdata=='\r'))
        break;
    sdata1[i1] = sdata;
    i1++;
    if (i1>=4)
        break;
 }
 text1 = "Please type number 2 and enter \n\r\0";
 USART1_putS(text1);

 flush_USART1();

 i2=0;
 while(1)
 {
    sdata = USART1_getC();
    if ((sdata=='\n') || (sdata=='\r'))
        break;
    sdata2[i2] = sdata;
    i2++;
    if (i2>=4)
        break;
 }

 if(i1==1)
    number1=toi(sdata1[0]);
 if(i1==2)
    number1=toi(sdata1[0])*10+toi(sdata1[1]);
 if(i1==3)
    number1=toi(sdata1[0])*100+toi(sdata1[1])*10+toi(sdata1[2]);
 if(i1==4)
 {
    number1=toi(sdata1[0])*1000+toi(sdata1[1])*100;
    number1=number1+toi(sdata1[2])*10+toi(sdata1[3]);
 }

 if(i2==1)
    number2=toi(sdata2[0]);
 if(i2==2)
    number2=toi(sdata2[0])*10+toi(sdata2[1]);
 if(i2==3)
    number2=toi(sdata2[0])*100+toi(sdata2[1])*10+toi(sdata2[2]);
 if(i2==4)
 {
    number2=toi(sdata2[0])*1000+toi(sdata2[1])*100;
    number2=number2+toi(sdata2[2])*10+toi(sdata2[3]);
 }

 result=number1+number2;

 text1 = "The summation result is: \0";
 USART1_putS(text1);

 temp = (int) (result/10000)%10;
 USART1_sendC(digits[temp]);
 temp = (int) (result/1000)%10;
 USART1_sendC(digits[temp]);
 temp = (int) (result/100)%10;
 USART1_sendC(digits[temp]);
 temp = (int) (result/10)%10;
 USART1_sendC(digits[temp]);
 temp = (int) (result)%10;
 USART1_sendC(digits[temp]);
 USART1_sendC('\n');
 USART1_sendC('\r');
 while(1);

}



 