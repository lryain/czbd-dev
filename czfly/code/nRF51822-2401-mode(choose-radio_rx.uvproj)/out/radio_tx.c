//arm_startup_nrf51.s�������ļ��������ж϶���
//#include "nrf51.h"//����51822�����Ĵ����ṹ��ַ������2�ļ��Ϳ��Կ���51822�ˡ�
//nrf51_bitfields.h���������Ĵ�������ֵ������
#include "project_nrf51822.c"//����51822������Ҫ�õ���ͷ�ļ���������ʱ��io��ʲô��

#include <stdbool.h>//����bool
#include "stdio.h"//��׼�������
#include <stdint.h>//������֪��С����������ʾ����������
#include "simple_uart.h"

//packet[3]��QxC(Q��ʼ��C������x������char)
static uint8_t packet[32];

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
    //������usb-jtag��nrf51822
//	  gpio_init_in(9,9,3);//p0.09����
//	  gpio_init_in(11,11,3);//p0.11����
//    gpio_init_out(21,23);//led:red,green,blue
	
    	
    	gpio_init_out(17,18);//����18~25������Ϊ���/�Ǹ�26~27�ӵ��ǵ��پ���

}





	int main(void)//main������ʼ
{
    init();//ʱ�ӳ�ʼ��
    ioinit();//���ų�ʼ��    
	  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);

	  radio_configure();//���ߵ����ã����ں�nrf2401ͨѶ��
	  // ������Ч�غ�ָ��
    NRF_RADIO->PACKETPTR = (uint32_t)packet;//ָ��ָ�����ݻ���packet  
	
	//���io���Ƿ�����
    //NRF_GPIO->OUTSET=(1<<17);//led4������ʾ51822�Ѿ�������
   //nrf_gpio_pin_set(18);
	while(1)
  {
	  int i,j,serial_falg; 
		packet[i++]=simple_uart_get();
		//nrf_gpio_pin_set(18);
		if(packet[0]==0xAB && packet[1]==0xBA)
		{	
			if(i==32)
			{
				i=0;
				serial_falg=0;
				for(j=20;j<32;j++)
				{
					if(packet[j]!='A')
					{
						serial_falg++;//ǰ��19�����ݷ��͵�nRF51���ڻ���[32]��С��ʣ�����0��һֱ����������
					}
        }
				if(serial_falg ==0)
				{
					//����׼������
					NRF_RADIO->EVENTS_READY = 0U;//�շ�ģʽ�л���ɱ�־λ����λ
					NRF_RADIO->TASKS_TXEN = 1U;//�������ߵ�Ϊ����ģʽ
							while (NRF_RADIO->EVENTS_READY == 0U);//�ȴ��շ�ģʽ�л����
							// �������ݴ���
					NRF_RADIO->EVENTS_END  = 0U;//���������־λ����λ    
					NRF_RADIO->TASKS_START = 1U;//��ʼ����		
					while(NRF_RADIO->EVENTS_END == 0U) //�ȴ��������
					{ 					
							nrf_gpio_pin_set(18);//����ȴ�led0��
					}
					nrf_gpio_pin_clear(18);//led0�𣬱�ʾ�����Ѿ�����
					// ��1���ţ�8~15��
					//nrf_gpio_port_write(P1, packet[1]);//��ֵ��ʾ�ڵ�Ƭ��P1����
					
					
					NRF_RADIO->EVENTS_DISABLED = 0U;//����رձ�־λ  ��λ
					NRF_RADIO->TASKS_DISABLE   = 1U; // �رմ���
					while(NRF_RADIO->EVENTS_DISABLED == 0U)//�ȴ�����ر�
					{
							// Do nothing.
					}
				}
			}
		}
		else
		{
			if(i==2)i=0;
		}
		
  }
}
