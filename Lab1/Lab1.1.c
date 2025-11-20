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
  // Define variable type GPIO 
  GPIO_InitTypeDef GPIO_InitStruct; 
  // Enable clock for GPIOC and GPIO B 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE); 
  // Configure PC13 as output push pull 50MHz 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13; 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStruct); 
   //  Configure PB1 and PB15 as output push pull 50MHz 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_15; 
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStruct); 
}

int main(void) 
 { 
   int i = 0 , temp = 0; 
   GPIO_Configuration(); 
   GPIO_WriteBit(GPIOC,GPIO_Pin_13,1); 
   GPIO_WriteBit(GPIOB,GPIO_Pin_1,1); 
   GPIO_WriteBit(GPIOB,GPIO_Pin_15,1); 
   while(1) 
   { 
    temp = i % 2; 
    GPIO_WriteBit(GPIOB,GPIO_Pin_1,temp); 
    GPIO_WriteBit(GPIOB,GPIO_Pin_15,!temp); 
    my_delay(400000); 
    i++; 
   } 
 }
