#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

void USART12_setup()
{
 USART_InitTypeDef usart_init_struct;
 /* Baud rate 115200, 8-bit data, One stop bit No parity, Do both Rx and Tx, No
 HW flow control*/
 usart_init_struct.USART_BaudRate = 115200;
 usart_init_struct.USART_WordLength = USART_WordLength_8b;
 usart_init_struct.USART_StopBits = USART_StopBits_1;
 usart_init_struct.USART_Parity = USART_Parity_No ;
 usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 /* Configure USART1 & 2 */
 USART_Init(USART1, &usart_init_struct);
 USART_Init(USART2, &usart_init_struct);
 /* Configure Interrupt */
 /* Disable Tx and Enable Rx */
 USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
 USART_Cmd(USART1, ENABLE);
 USART_Cmd(USART2, ENABLE);
}

void USART2_NVIC_Config(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;
 // Priority grouping
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
 // Configure interrupt priority
 NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_Init(&NVIC_InitStructure);
}

void USART1_sendC(unsigned char c)
{
 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
 USART_SendData(USART1,(unsigned char) c);
}

void USART2_sendC(unsigned char c)
{
 while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
 USART_SendData(USART2,(unsigned char) c);
}
 
void USART1_putS(unsigned char *s)
{
 while (*s) {
    USART1_sendC(*s);
    s++;
 }
}
 
void USART2_putS(unsigned char *s)
{
  while (*s) {
    USART2_sendC(*s);
    s++;
 }
}

void GPIO_Configuration()
{
 // Define variable type GPIO
 GPIO_InitTypeDef GPIO_InitStruct;
 // Enable clock for GPIOC and GPIO A and GPIO B and AFIO and USART1 and USART2
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA , ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
 // Configure PC13 as output push pull 50Hz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOC, &GPIO_InitStruct);
 GPIO_WriteBit(GPIOC,GPIO_Pin_13,0);
 // Configure PA0 and PA1 as Analog input
 GPIO_InitStruct.GPIO_Pin= GPIO_Pin_0 | GPIO_Pin_1;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 // Configure PA9 Tx for USART1 and PA2 Tx for USART2
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 // Configure PA10 Rx for USART1 and PA3 Rx for USART2
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOA, &GPIO_InitStruct);

 // Configure PB1 and PB15 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_1;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
 // Configure PB3, PB4, PB8 and P9 as input floating
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* Public declaration */
 #define RX_BUF_SIZE 512
 #define RX_BUF_MASK RX_BUF_SIZE-1
 volatile char rx_buf[RX_BUF_SIZE]="";
 volatile u16 rx_pos=0;

void USART2_IRQHandler(void)
{
 unsigned char rx_b;
 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
 {
 rx_b = USART_ReceiveData(USART2);
 rx_buf[rx_pos++]=rx_b;
 rx_pos &= RX_BUF_MASK;
 }
 rx_buf[rx_pos+1]='\0';
}

void clear_rx_buf(void)
{
 for(;rx_pos!=0;rx_pos--) rx_buf[rx_pos]='\0';
}

void DelayMC(unsigned int ms)
{
 volatile unsigned int nCount;
 nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
 while(nCount--);
}


/* ---- ESP 01 Predefined AT Protocol ----- */
const char AT_test[] = "AT\r\n";
const char echoOff[] = "ATE0\r\n";
const char restore[] = "AT+RESTORE\r\n"; //Restore factory settings
const char uart_baud [] = "AT+UART=115200,8,1,0,0\r\n" ;//Baudrate
const char list_AP [] = "AT+CWLAP\r\n";
const char station_mode [] = "AT+CWMODE=1\r\n"; //station mode
const char conect_to_AP [] = "AT+CWJAP=\"Tenda_48B2E0\",\"1234567890\"\r\n";
const char disconectAP [] = "AT+CWQAP\r\n";
const char conect_AP_status [] = "AT+CWJAP?\r\n";
const char firmware_version [] = "AT+GMR\r\n";
const char NoMultConn[] = "AT+CIPMUX=0\r\n";
const char rst [] = "AT+RESTORE\r\n";
const char get_IP [] = "AT+CIFSR\r\n";
const char send_TSP [] = "AT+CIPSEND=";

int main(void)
{
 unsigned char * msg;
 unsigned int ADCRead_value = 0 , i = 0 ;
 static const char digits [] = "0123456789ABCDEF";
 unsigned char adc_msg[] = "xxxx\r\n\n\0";
 GPIO_Configuration();
 USART12_setup();
 USART2_NVIC_Config();
 USART1_putS("< Configuration Finish > \n\r");
 USART1_putS("Start Connecting to ESP01\r\n\0");
 USART1_putS("Waiting for Echo \n\r");

 USART2_putS(AT_test);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(firmware_version);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(station_mode);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(list_AP);
 DelayMC(200);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(conect_to_AP);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(conect_AP_status);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(get_IP);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

 USART2_putS(NoMultConn);
 DelayMC(20);
 waitforOK();
 USART1_putS(rx_buf);
 clear_rx_buf();

while(1)
{
}
}
