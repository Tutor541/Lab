#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define PCF8591_7BIT_ADDR 0x48

void DelayMC(unsigned int ms)
{
 volatile unsigned int nCount;
 nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
 while(nCount--);
}

void GPIO_Configuration()
{
 GPIO_InitTypeDef GPIO_InitStruct;
 // Enable clock for GPIOA and GPIOB and AFIO and USART1
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1,ENABLE);
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
 /* I2C1 SDA and SCL configuration */
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Initial_I2C1()
{
 I2C_InitTypeDef i2c;
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
 I2C_DeInit(I2C1);
 i2c.I2C_ClockSpeed = 100000;
 i2c.I2C_Mode = I2C_Mode_I2C;
 i2c.I2C_DutyCycle = I2C_DutyCycle_2;
 i2c.I2C_OwnAddress1 = 0x4E;
 i2c.I2C_Ack = I2C_Ack_Enable;
 i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
 I2C_Cmd(I2C1, ENABLE);
 I2C_Init(I2C1, &i2c);
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

void SCAN_I2C1()
{
 unsigned char i =0;
 unsigned int timeout = 100000;
 USART1_putS(" <I2C 1 Searching Loop> \n\r");
 unsigned char buf[20];
 int a=0;
 for ( i=5;i<128;i++)
 {
    USART1_putS(" Scaning at Address 0x");
    itoa(i,buf,16);
    USART1_putS(buf);
    USART1_putS("\n\r");
    I2C_ClearFlag(I2C1,I2C_FLAG_AF);
    I2C_AcknowledgeConfig(I2C1,ENABLE);
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);

    I2C_Send7bitAddress(I2C1, i <<1 |0, I2C_Direction_Transmitter);
    // check if slave acknowledged his address within timeout
    while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    && timeout )
    {
        timeout--;
    }
    DelayMC(50);
    if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != 0x0 );
    if(timeout == 0x0 )
    { I2C_GenerateSTOP(I2C1, ENABLE); }
    else
    {
     USART1_putS(">>>>>>>>> Found device at 0x");
     itoa(i,buf,16);
     USART1_putS(buf);
     USART1_putS(" <<<<<<<<<<<<\n\r");
     I2C_GenerateSTOP(I2C1, ENABLE);
    }
    DelayMC(50);
    timeout = 100000;
   }
 USART1_putS("<I2C 1 Search End> \n\r");
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

void write_I2C1(uint8_t addr, uint8_t data)
{
 I2C_ClearFlag(I2C1,I2C_FLAG_AF);
 while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
 I2C_GenerateSTART(I2C1, ENABLE);
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
 I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
 I2C_SendData(I2C1, data);
 while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
 I2C_GenerateSTOP(I2C1, ENABLE);
 while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
}

uint8_t read_I2C1(uint8_t addr)
{
 uint8_t i2c_data;
 I2C_AcknowledgeConfig(I2C1, ENABLE);
 while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
 I2C_GenerateSTART(I2C1, ENABLE);
 while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB));
 I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Receiver);
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
 i2c_data = I2C_ReceiveData(I2C1);
 I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
 I2C_AcknowledgeConfig(I2C1, DISABLE);
 I2C_GenerateSTOP(I2C1, ENABLE);
 while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
 return i2c_data;
}

void write_dac_I2C1(uint8_t addr, uint8_t data)
{
 I2C_ClearFlag(I2C1,I2C_FLAG_AF);
 while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
 I2C_GenerateSTART(I2C1, ENABLE);
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
 I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
 I2C_SendData(I2C1, 0x40);
 while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
 I2C_SendData(I2C1, data);
 while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
 I2C_GenerateSTOP(I2C1, ENABLE);
 while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
}

int main(void)
{
 unsigned char i = 0;
 GPIO_Configuration();
 USART_setup();
 Initial_I2C1();
 DelayMC(100);
 //Send Control word to PCF8591 //

 while(1)
 {
  write_dac_I2C1( PCF8591_7BIT_ADDR <<1 |0 , i);
  i++;
  DelayMC(20);
 }
}