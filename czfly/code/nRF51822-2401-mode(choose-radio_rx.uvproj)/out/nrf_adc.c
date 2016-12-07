#include "nrf_adc.h"

/** @brief Function for initializing the ADC peripheral.
*/
void nrf_adc_init_vbat(void)
{
	NRF_ADC->CONFIG =0x406;//ADC精度10bit，预分频2/3，使用内部带隙1.2V基准，AIN2引脚输入，模拟参考引脚输入禁用。
	NRF_ADC ->ENABLE=1;//使能ADC
	//NRF_ADC->INTENSET=1;//打开ADC中断
	//NVIC_SetPriority(ADC_IRQn ,1);
	//NVIC_EnableIRQ(ADC_IRQn);
}

uint32_t nrf_adc_read(void)
{
  uint32_t adc_data;
  NRF_ADC->TASKS_START = 1;
  while(NRF_ADC->EVENTS_END == 0);
  NRF_ADC->EVENTS_END = 0;
  adc_data = NRF_ADC->RESULT;
  return adc_data;
}

float nrf_adc_read_vbat_f(void)
{
    return (float)nrf_adc_read() * 3.0f * 3.0f * 1.2f / 2.0f /1024.0f;
}

uint32_t nrf_adc_read_vbat_mv(void)
{
    // The non-optimized math is: (ADC / 1024 /2) * 3 * 3 * 1.2 * 1000
    return nrf_adc_read() / 1024 * (3 * 3 * 600) ;
}
