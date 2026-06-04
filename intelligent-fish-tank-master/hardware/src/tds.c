
#include "tds.h"

uint16_t ADC_ConvertedValue ;
//PB0 3.3v或5v  ppm
void tds_init(void)
{
	GPIO_InitTypeDef Gpio_Value;
	ADC_InitTypeDef Adc_Value;
	DMA_InitTypeDef Dma_Value;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	Gpio_Value.GPIO_Mode = GPIO_Mode_AIN;
	Gpio_Value.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB,&Gpio_Value);
	
	Adc_Value.ADC_ContinuousConvMode = ENABLE;
	Adc_Value.ADC_DataAlign = ADC_DataAlign_Right;
	Adc_Value.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	Adc_Value.ADC_Mode = ADC_Mode_Independent;
	Adc_Value.ADC_NbrOfChannel = 1;
	Adc_Value.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1,&Adc_Value);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_55Cycles5);
	
	
	DMA_DeInit(DMA1_Channel1);
	Dma_Value.DMA_BufferSize = 1;
	Dma_Value.DMA_DIR = DMA_DIR_PeripheralSRC; //adc作为源地址
	Dma_Value.DMA_M2M = DMA_M2M_Disable;
	Dma_Value.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; 
	Dma_Value.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	Dma_Value.DMA_MemoryInc = DMA_MemoryInc_Disable;
	Dma_Value.DMA_Mode = DMA_Mode_Circular;
	Dma_Value.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADC1->DR ) ); 
	Dma_Value.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	Dma_Value.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	Dma_Value.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1,&Dma_Value);
	//使能
	ADC_Cmd(ADC1,ENABLE);					
	DMA_Cmd(DMA1_Channel1,ENABLE);
	ADC_DMACmd(ADC1,ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC1);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	// ADC开始校准
	ADC_StartCalibration(ADC1);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

float get_tds_value(void)
{
	float TDS_voltage=0.0;
	float compensationVolatge = 0.0;
	float compensationCoefficient=1.0;//温度校准系数
	float TDS_value=0.0; //tds的值
	float kValue=1.0;
	TDS_voltage =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
	//printf("%d\r\n",ADC_ConvertedValue);
	compensationCoefficient=1.0+0.02*((25/10)-25.0); 
	compensationVolatge=TDS_voltage/compensationCoefficient;
	TDS_value=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 
	255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5*kValue;
	
	if(TDS_value<=0)
	{
		TDS_value=0;
	}
	if(TDS_value>1400)
	{
		TDS_value=1400;
	}

	return TDS_value;

}










