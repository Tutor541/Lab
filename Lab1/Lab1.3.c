#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void my_delay(unsigned int count) 
{ 
    while(count>0) 
    {   count--;  } 
}

void DelayMC(unsigned int ms)
{
 volatile unsigned int nCount;
 nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
 while(nCount--);
}

void GPIO_Configuration()
{
 // Define variable type GPIO
 GPIO_InitTypeDef GPIO_InitStruct;
 // Enable clock for GPIOA and GPIO B
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB , ENABLE);
 // Configure PB12 and PB13 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
 // Configure PA3,PA4 and PA8,PA9 output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void pulse_LCD()
{
 // E --> PB12
 DelayMC(2);
 GPIO_WriteBit(GPIOB,GPIO_Pin_12,1);
 DelayMC(2);
 GPIO_WriteBit(GPIOB,GPIO_Pin_12,0);
 DelayMC(2);
}

void LCD_4bits_Init()
 {
 GPIO_WriteBit(GPIOB,GPIO_Pin_13,0);
 DelayMC(120);
 //send 0x30 1st time
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();

 DelayMC(80);
 //send 0x30 2nd time
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();

 DelayMC(50);
 //send 0x30 3rd time
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();

 DelayMC(50);
 //send 0x20) to make LCD 4bits mode
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();

 //-------------LCD_Config------------//
 DelayMC(50);
 //send function mode set
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,1);
 pulse_LCD();
 //send LCD Cursor display set
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,1);
 pulse_LCD();
 //send Entry Mode set
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 //send clear LCD command
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 //send LCD Cursor display set
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,1);
 pulse_LCD();
 //send clear LCD command
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
 GPIO_WriteBit(GPIOA,GPIO_Pin_4,1);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,0);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,0);
 pulse_LCD();
}

void LCD_send_data(unsigned int _data)
{

 unsigned char data_bit[4];
 unsigned char data_4bitlow, data_4bithigh;
 data_4bithigh=_data&0xf0;
 data_4bitlow=(_data<<4)&0xf0;

 GPIO_WriteBit(GPIOB,GPIO_Pin_13,1);

 data_bit[0] = ( ((data_4bithigh>>4) & 0x01) ? 1 :0 ) ;
 data_bit[1] = ( ((data_4bithigh>>5) & 0x01) ? 1 :0 ) ;
 data_bit[2] = ( ((data_4bithigh>>6) & 0x01) ? 1 :0 ) ;
 data_bit[3] = ( ((data_4bithigh>>7) & 0x01) ? 1 :0 ) ;

 GPIO_WriteBit(GPIOA,GPIO_Pin_4,data_bit[0]);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,data_bit[1]);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,data_bit[2]);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,data_bit[3]);
 pulse_LCD();

 data_bit[0] = ( ((data_4bitlow>>4) & 0x01) ? 1 :0 ) ;
 data_bit[1] = ( ((data_4bitlow>>5) & 0x01) ? 1 :0 ) ;
 data_bit[2] = ( ((data_4bitlow>>6) & 0x01) ? 1 :0 ) ;
 data_bit[3] = ( ((data_4bitlow>>7) & 0x01) ? 1 :0 ) ;

 GPIO_WriteBit(GPIOA,GPIO_Pin_4,data_bit[0]);
 GPIO_WriteBit(GPIOA,GPIO_Pin_5,data_bit[1]);
 GPIO_WriteBit(GPIOA,GPIO_Pin_6,data_bit[2]);
 GPIO_WriteBit(GPIOA,GPIO_Pin_7,data_bit[3]);
 pulse_LCD();

 GPIO_WriteBit(GPIOB,GPIO_Pin_13,0);
}

int main(void)
{
 int i =0, state = 0;
 unsigned char text[]={84,104,73,115,32,105,83,32,65,32,76,97,66,'\0'};
 GPIO_Configuration();
 GPIO_WriteBit(GPIOB,GPIO_Pin_12,0);
 GPIO_WriteBit(GPIOB,GPIO_Pin_13,0);
 DelayMC(100);
 LCD_4bits_Init();
 DelayMC(100);
 while (text[i] != '\0')
 {
  LCD_send_data(text[i]);
    i++;
 }

 while(1);

}

