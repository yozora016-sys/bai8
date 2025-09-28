#include "stm32f10x.h"
#include <stdio.h>

void GPIO_Config(void);
void USART1_Config(void);
void ADC1_Config(void);
void USART1_SendString(char *str);

int main(void) {
    uint16_t adc_value;
    int voltage;
    char buffer[50];

    GPIO_Config();
    USART1_Config();
    ADC1_Config();

    while (1) {
        //bat dau chuyen doi adc
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        //cho den khi co eoc =1
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
			
				//doc ket qua tu thanh ghi dr, doc xong thi xoa co eoc
        adc_value = ADC_GetConversionValue(ADC1);
				
				//chuyen du lieu adc doc duoc sang 10bit
				//uint16_t adc_value_10bit = adc_value >> 2;

				//v= gt adc doc duoc* vref / (2^12-1)
        voltage = (adc_value * 3300) / 4095;  
			
				//in ra terminal
        sprintf(buffer, "ADC = %d, Voltage = %d mV\r\n", adc_value, voltage);
        USART1_SendString(buffer);
				
				//delay
        for (volatile int i = 0; i < 1000000; i++); // delay 
    }
}

void GPIO_Config(void) {
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA0 - Analog input (ADC Channel 0)
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AIN;//analog input
    GPIO_Init(GPIOA, &gpio);
}

void USART1_Config(void) {
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

		//bat clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // PA9 (TX)
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;//Alternate Function Push-Pull
    GPIO_Init(GPIOA, &gpio);

    // PA10 (RX)
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    // UART1
    usart.USART_BaudRate = 9600;//toc do bit
    usart.USART_WordLength = USART_WordLength_8b;//khung du lieu = 8bit
    usart.USART_StopBits = USART_StopBits_1;//1 stop bit
    usart.USART_Parity = USART_Parity_No;//khong dung bit chan le
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//chi dung rx tx de dieu khien
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//bat tx(truyen dl), rx(nhan dl)
    USART_Init(USART1, &usart);//init

    USART_Cmd(USART1, ENABLE);//bat uart1
}

void ADC1_Config(void) {
    ADC_InitTypeDef adc;
		//bat clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		//adcclk<=14khz, pclk2 tren stm32 =72khz => /6 =12khz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADCCLK = PCLK2/6

    adc.ADC_Mode = ADC_Mode_Independent;// chi dung adc1
    adc.ADC_ScanConvMode = DISABLE;// tat scan mode
    adc.ADC_ContinuousConvMode = DISABLE;// tat continuous mode
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// khong dung trigger ngoai, moi lan active dung dong 20
    adc.ADC_DataAlign = ADC_DataAlign_Right;// du lieu nam ben phai
    adc.ADC_NbrOfChannel = 1;//1 kenh
    ADC_Init(ADC1, &adc);

    // cau hinh kenh ADC0 (PA0)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);//thoi gian lay mau = 55.5 cycle

    ADC_Cmd(ADC1, ENABLE);// bat adc1

    // hieu chinh ADC
		//xoa du lieu cu trong adc
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
		
		//bat dau calibration moi
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

//ham gui chuoi
void USART1_SendString(char *str) {
    while (*str) {
        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}

