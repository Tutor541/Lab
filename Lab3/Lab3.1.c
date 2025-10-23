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
 // Configure PA0 and PA1 as Analog input
 GPIO_InitStruct.GPIO_Pin= GPIO_Pin_0 | GPIO_Pin_1;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
 GPIO_Init(GPIOA, &GPIO_InitStruct);
 // Configure PB1 and PB15 as output push pull 50MHz
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_15;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void ADC1_Init(void)
 {
 ADC_InitTypeDef ADC_InitStructure;
 /* PCLK2 is the APB2 clock */
 /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
 RCC_ADCCLKConfig(RCC_PCLK2_Div6);
 /* Enable ADC1 clock */
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
 /* Put everything back to power-on defaults */
 ADC_DeInit(ADC1);
 /* ADC1 Configuration */
 /* ADC1 and ADC2 operate independently */
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
 /* scan conversion >> but #channel is one and controlled by software */
 ADC_InitStructure.ADC_ScanConvMode = ENABLE;
 /* continuous conversions >> #channel is one and controlled by software */
 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
 /* Start conversion by software, not an external trigger */
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
 /* Conversions are 12 bit - in the lower 12 bits of the result */
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
 /* Only one Channel in sequence */
 ADC_InitStructure.ADC_NbrOfChannel = 1;
 /* Initialize to ADC1 */
 ADC_Init(ADC1, &ADC_InitStructure);
 /* Enable ADC1 */
 ADC_Cmd(ADC1, ENABLE);
 /* Enable ADC1 reset calibaration register */
 ADC_ResetCalibration(ADC1);
 /* Check the end of ADC1 reset calibration register */
 while(ADC_GetResetCalibrationStatus(ADC1));
 /* Start ADC1 calibration */
 ADC_StartCalibration(ADC1);
 /* Check the end of ADC1 calibration */
 while(ADC_GetCalibrationStatus(ADC1));
 }

 unsigned int readADC1(unsigned char channel)
 {
 ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_13Cycles5);
 // Start the conversion
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);
 // Wait until conversion completion
 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
 // Get the conversion value
 return ADC_GetConversionValue(ADC1);
 }

  int main(void)
 {
 unsigned int ADCRead_value = 0;
 GPIO_Configuration();
 GPIO_WriteBit(GPIOB,GPIO_Pin_1,1);
 GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
 ADC1_Init();
 while(1)
 {
 ADCRead_value = readADC1(1);
 if( ADCRead_value >= (int) (4096/2) )
 {
 GPIO_WriteBit(GPIOB,GPIO_Pin_1,0);
 GPIO_WriteBit(GPIOB,GPIO_Pin_15,1);
 }
 else
 {
 GPIO_WriteBit(GPIOB,GPIO_Pin_1,1);
 GPIO_WriteBit(GPIOB,GPIO_Pin_15,0);
 }
 DelayMC(5);
 }
 }