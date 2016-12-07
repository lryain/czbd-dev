//arm_startup_nrf51.s�������ļ��������ж϶���
//#include "nrf51.h"//����51822�����Ĵ����ṹ��ַ������2�ļ��Ϳ��Կ���51822�ˡ�
//nrf51_bitfields.h���������Ĵ�������ֵ������
#include "project_nrf51822.c"//����51822������Ҫ�õ���ͷ�ļ���������ʱ��io��ʲô��

#include <stdbool.h>//����bool
#include "stdio.h"//��׼�������
#include <stdint.h>//������֪��С����������ʾ����������


void init(void)//51822ʱ�ӳ�ʼ��
{ 
	 //�жϵ�Դʱ��״̬
	 if ((((*(uint32_t *)0xF0000FE0) & 0xFF) == 1) && (((*(uint32_t *)0xF0000FE4) & 0xF) == 0))
    {
        if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x0) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
					//���radio�޷�д��  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//��Դʱ������
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE����    
        }
				else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x10) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //���radio�޷�д��  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//��Դʱ������
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE����  
        }
        else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x30) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //���radio�޷�д��  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//��Դʱ������
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE����    
        }
    }
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;//HFCLK��������״̬0�رգ�1����
  NRF_CLOCK->TASKS_HFCLKSTART = 1;//���⾧��HFCLK 0�رգ�1����
  while(!NRF_CLOCK->EVENTS_HFCLKSTARTED);//�ȴ�HFCLK��������״̬Ϊ1
}

void ioinit()//51822���ų�ʼ��
{
   gpio_init_out(17,18);//����0~15������Ϊ���
}


//packet[3]��QxC(Q��ʼ��C������x������char)
static uint8_t volatile packet[32];  //< ���ݴ���Ļ����packet[1]������

int main(void)//main������ʼ
{
    int i;
	  uint32_t  RF_timeOver_flag;
    float vbatLi;//�����صı�������
	  PMStates pmStateOld = battery,pmState; //��س�������صı������� 
	  init();//ʱ�ӳ�ʼ��
    ioinit();//���ų�ʼ��
	  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
	  radio_configure();//���ߵ����ã�Ҳ�����ں�nrf2401ͨѶ��
	  nrf_adc_init_vbat();
	  pmInit();
	  pmSetChargeState(charge500mA);
 
	  // ������Ч�غ�ָ��
    NRF_RADIO->PACKETPTR    = (uint32_t)packet;//ָ��ָ�����ݻ���packet
    nrf_gpio_pin_clear(17);
	  nrf_gpio_pin_clear(18);
    while(1)
    {
			  pmState=pmUpdateState();
			  //vbatLi=nrf_adc_read_vbat_f(); 
			  NRF_RADIO->EVENTS_READY = 0U; //�շ�ģʽת�����  ��־λ       
        NRF_RADIO->TASKS_RXEN   = 1U; //����ģʽ
        while(NRF_RADIO->EVENTS_READY == 0U) //�ȴ��շ�ģʽת�����(����ģʽ)��־λ
          {
            // Do nothing.�ȴ�
          }
        NRF_RADIO->EVENTS_END  = 0U;//�������  ��־λ     
        NRF_RADIO->TASKS_START = 1U; // ��ʼ����

        while(NRF_RADIO->EVENTS_END == 0U)//�ȴ��������  ��־λ
           {
						 // Do nothing.�ȴ�
//              RF_timeOver_flag++;
// 						 if(RF_timeOver_flag>500000)break;//�����ʱ��û�н��յ��������ݰ�������whileѭ��������������Ӱ�������������У�����
           }
        if (NRF_RADIO->CRCSTATUS == 1U)//���CRCУ����ȷ
        {					
						if(packet [0]==0xAB && packet [1]==0xBA)
						{
								packet[11]=nrf_adc_read() & 0x00ff;
							  packet[12]=nrf_adc_read()>>8;
							  for(i=0;i<32;i++)
								{
									simple_uart_put(packet[i]);
								}		
					  }						
        }

        NRF_RADIO->EVENTS_DISABLED = 0U;//���߹ر�   ��־λ
        NRF_RADIO->TASKS_DISABLE   = 1U;// �ر������豸

        while(NRF_RADIO->EVENTS_DISABLED == 0U)//�ȴ��豸�ر�
        {
            // Do nothing.
        }
    }   
}

