#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void my_delay(unsigned int count) 
{ 
    while(count>0) 
    {   count--;  } 
} 

void GPIO_Configuration()
{
 GPIO_InitTypeDef GPIO_InitStruct;
 // Enable clock for GPIOC and GPIOB and AFIO and Disable JTAG Mode
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
 // Configure PC13 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOC, &GPIO_InitStruct);
 // Configure PB1 and PB15 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_15;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
 // Configure PB3,PB4 and PB8,PB9 as input floating
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
}

int main(void)
{
 int state = 0 , ReadValue = 0;
 GPIO_Configuration();
 GPIO_WriteBit(GPIOC,GPIO_Pin_13,1);
 GPIO_WriteBit(GPIOB,GPIO_Pin_1,1);
 GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
 while(1)
 {
  ReadValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
  if (ReadValue==0)
    GPIO_WriteBit(GPIOB,GPIO_Pin_1,1);
  ReadValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
  if (ReadValue==0)
    GPIO_WriteBit(GPIOB,GPIO_Pin_1,0);
  ReadValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
  if (ReadValue==0)
    GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
  ReadValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
  if (ReadValue==0)
    GPIO_WriteBit(GPIOB,GPIO_Pin_15,0);
  if (state == 0) {
    GPIO_WriteBit(GPIOC,GPIO_Pin_13,1);
    state = 1;
  }
  if (state == 1) {
    GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
    state = 0;
  }
  my_delay(400000);
  }
}
