#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f1xx_it.h"

/* ================= UART & NVIC ================= */

void USART12_setup()
{
USART_InitTypeDef usart_init_struct;
/* Baud rate 115200, 8-bit data, One stop bit No parity, Do both Rx and Tx, No HW flow control */
usart_init_struct.USART_BaudRate = 115200;
usart_init_struct.USART_WordLength = USART_WordLength_8b;
usart_init_struct.USART_StopBits = USART_StopBits_1;
usart_init_struct.USART_Parity = USART_Parity_No ;
usart_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

/* Configure USART1 & 2 */
USART_Init(USART1, &usart_init_struct);
USART_Init(USART2, &usart_init_struct);

/* Enable RX interrupt on USART2 */
USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

USART_Cmd(USART1, ENABLE);
USART_Cmd(USART2, ENABLE);
}

void USART2_NVIC_Config(void)
{
NVIC_InitTypeDef NVIC_InitStructure;
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);
}

/* ================= UART send helpers ================= */

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
while (*s)
{
USART1_sendC(*s);
s++;
}
}

void USART2_putS(unsigned char *s)
{
while (*s)
{
USART2_sendC(*s);
s++;
}
}

/* ================= GPIO ================= */

void GPIO_Configuration()
{
GPIO_InitTypeDef GPIO_InitStruct;

/* Enable clocks */
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA , ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 , ENABLE);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

/* PC13 as output (LED on KU board) */
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(GPIOC, &GPIO_InitStruct);
GPIO_WriteBit(GPIOC, GPIO_Pin_13, 0);

/* PA0, PA1 as analog input (VR0, VR1) */
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
GPIO_Init(GPIOA, &GPIO_InitStruct);

/* PA9 (TX1), PA2 (TX2) AF push-pull */
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
GPIO_Init(GPIOA, &GPIO_InitStruct);

/* PA10 (RX1), PA3 (RX2) input floating */
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOA, &GPIO_InitStruct);

/* PB1, PB15 as output (LEDs) */
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_15;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(GPIOB, &GPIO_InitStruct);

/* PB3, PB4, PB8, PB9 as input floating (switches / others) */
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_9;
GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* ================= RX buffer & ISR ================= */

#define RX_BUF_SIZE 512
#define RX_BUF_MASK (RX_BUF_SIZE-1)

volatile char rx_buf[RX_BUF_SIZE] = "";
volatile u16 rx_pos = 0;

void USART2_IRQHandler(void)
{
unsigned char rx_b;
if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
{
rx_b = USART_ReceiveData(USART2);
rx_buf[rx_pos++] = rx_b;
rx_pos &= RX_BUF_MASK;
}
rx_buf[rx_pos+1] = '\0';
}

void clear_rx_buf(void)
{
for(; rx_pos != 0; rx_pos--)
rx_buf[rx_pos] = '\0';
rx_pos = 0;
}

/* ================= Delay ================= */

void DelayMC(unsigned int ms)
{
volatile unsigned int nCount;
nCount = (unsigned int)((72000000/4000000)*ms*1000 - 10);
while(nCount--);
}

/* ================= ADC ================= */

void ADC1_Init(void)
{
ADC_InitTypeDef ADC_InitStructure;

RCC_ADCCLKConfig(RCC_PCLK2_Div6); /* 72/6 = 12 MHz */
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

ADC_DeInit(ADC1);

ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
ADC_InitStructure.ADC_ScanConvMode = ENABLE;
ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
ADC_InitStructure.ADC_NbrOfChannel = 1;
ADC_Init(ADC1, &ADC_InitStructure);

ADC_Cmd(ADC1, ENABLE);

ADC_ResetCalibration(ADC1);
while(ADC_GetResetCalibrationStatus(ADC1));
ADC_StartCalibration(ADC1);
while(ADC_GetCalibrationStatus(ADC1));
}

unsigned int readADC1(unsigned char channel)
{
ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_13Cycles5);
ADC_SoftwareStartConvCmd(ADC1, ENABLE);
while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
return ADC_GetConversionValue(ADC1);
}

/* ================= Wait helpers (OK, >, +IPD) ================= */

void waitforOK(void)
{
unsigned char ch1 , ch2;
unsigned char check = 0;

while(check == 0)
{
if (rx_pos >= 4)
{
ch1 = rx_buf[rx_pos-4];
if (ch1 == 'O')
{
while(1)
{
ch2 = rx_buf[rx_pos-3];
if (ch2 == 'K')
{
check = 1;
}
if (check == 1) break;
}
}
if(check == 1) break;
}
}
}

void waitforPromp(void)
{
unsigned char ch1;
DelayMC(80);
while(1) /* wait until get '>' from ESP8266 USART2 */
{
ch1 = rx_buf[rx_pos-2];
if(ch1 == '>')
{
break;
}
}
}

/* --- HOMEWORK: waitforIPD(timeout) --- */
/* return 1 = found +IPD , 0 = timeout */

unsigned char waitforIPD(unsigned int timeout_ms)
{
unsigned char ch1 , ch2 , ch3;
unsigned char check = 0;
unsigned int i;
unsigned int t = 0;

while((check == 0) && (t < timeout_ms))
{
if (rx_pos >= 6)
{
i = 0;
while(i < (rx_pos+2))
{
ch1 = rx_buf[rx_pos-i];
if (ch1 == 'D')
{
ch2 = rx_buf[rx_pos-i-1];
if (ch2 == 'P')
{
ch3 = rx_buf[rx_pos-i-2];
if (ch3 == 'I')
{
check = 1;
break;
}
}
}
i++;
}
}
if(check == 1) break;

/* 1 ms polling step */
DelayMC(1);
t++;
}

DelayMC(50); /* same tail-delay asเดิม */
return check;
}

/* ================= AT command strings ================= */

const char AT_test[] = "AT\r\n";
const char echoOff[] = "ATE0\r\n";
const char restore[] = "AT+RESTORE\r\n"; // Restore factory settings
const char uart_baud [] = "AT+UART=115200,8,1,0,0\r\n";
const char list_AP [] = "AT+CWLAP\r\n";
const char station_mode [] = "AT+CWMODE=1\r\n"; // station mode
const char conect_to_AP [] = "AT+CWJAP=\"Tenda_48B2E0\",\"1234567890\"\r\n";
const char disconectAP [] = "AT+CWQAP\r\n";
const char conect_AP_status [] = "AT+CWJAP?\r\n";
const char firmware_version [] = "AT+GMR\r\n";
const char NoMultConn[] = "AT+CIPMUX=0\r\n";
const char rst [] = "AT+RESTORE\r\n";
const char get_IP [] = "AT+CIFSR\r\n";
const char send_TSP [] = "AT+CIPSEND=";

/* ================= MAIN : combined 8.3 + 8.4 ================= */

int main(void)
{
unsigned char * msg;
unsigned int ADCRead0 = 0, ADCRead1 = 0;
unsigned int i = 0;
static const char digits[] = "0123456789ABCDEF";

/* "xxxx yyyy\r\n" (11 bytes + '\0') */
unsigned char adc_msg[12] = "0000 0000\r\n";

GPIO_Configuration();
USART12_setup();
USART2_NVIC_Config();
ADC1_Init();

USART1_putS("< Configuration Finish > \r\n");
USART1_putS("Start Connecting to ESP01\r\n");
USART1_putS("Waiting for Echo \r\n");

/* ---- Standard init (เหมือน 8.2/8.3) ---- */

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

/* ---- เปิด TCP connection ไป server ---- */
msg = "AT+CIPSTART=\"TCP\",\"192.168.34.122\",80\r\n\0"; // แก้ IP ให้ตรงเครื่องตัวเอง
USART2_putS(msg);
DelayMC(20);
waitforOK();
USART1_putS(rx_buf);
clear_rx_buf();

while(1)
{
/* --------- 1) อ่าน VR0, VR1 ส่งขึ้น TCP server (ส่วนของ 8.3) --------- */

ADCRead0 = readADC1(0); // VR0 (PA0)
ADCRead1 = readADC1(1); // VR1 (PA1)

adc_msg[0] = digits[(int)(ADCRead0 /1000)%10];
adc_msg[1] = digits[(int)(ADCRead0 /100)%10];
adc_msg[2] = digits[(int)(ADCRead0 /10)%10];
adc_msg[3] = digits[(int)(ADCRead0 /1)%10];

adc_msg[4] = ' ';

adc_msg[5] = digits[(int)(ADCRead1 /1000)%10];
adc_msg[6] = digits[(int)(ADCRead1 /100)%10];
adc_msg[7] = digits[(int)(ADCRead1 /10)%10];
adc_msg[8] = digits[(int)(ADCRead1 /1)%10];

adc_msg[9] = '\r';
adc_msg[10] = '\n';
adc_msg[11] = '\0';

USART1_putS("ADC0 ADC1 : ");
USART1_putS(adc_msg);

/* ขอส่ง 11 byte ไป server */
msg = "AT+CIPSEND=11\r\n\0";
USART2_putS(msg);
DelayMC(20);
waitforPromp(); // รอ '>'
clear_rx_buf();

USART2_putS(adc_msg); // ส่งค่าจริง
DelayMC(20);
waitforOK();
USART1_putS(rx_buf);
clear_rx_buf();

/* --------- 2) รอ command จาก server (ส่วนของ 8.4) --------- */
USART1_putS("Wait for Command\r\n");

if (waitforIPD(500)) // รอสูงสุด 500 ms ถ้าไม่มี ก็ข้ามรอบนี้
{
USART1_putS("Got Command\r\n");
USART1_putS("--------------------\r\n");
USART1_putS(rx_buf);
USART1_putS("\r\n--------------------\r\n");
USART1_putS("Executing Command\r\n");

i = 0;
while(1)
{
if (rx_buf[i] != ':')
i++;
else
break;
}

if(rx_buf[i+1] == 'L')
{
if(rx_buf[i+2]=='1') GPIO_WriteBit(GPIOB, GPIO_Pin_1, 1);
if(rx_buf[i+2]=='2') GPIO_WriteBit(GPIOB, GPIO_Pin_15, 1);
if(rx_buf[i+2]=='3') GPIO_WriteBit(GPIOB, GPIO_Pin_1, 0);
if(rx_buf[i+2]=='4') GPIO_WriteBit(GPIOB, GPIO_Pin_15, 0);
}

USART1_putS("\r\n");
clear_rx_buf();
}
else
{

}

DelayMC(200);
}
}
