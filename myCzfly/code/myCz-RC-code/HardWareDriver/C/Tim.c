#include "tim.h"
#include "stm32f10x_tim.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "NRF24L01.h"
#include "Led.h"
#include "misc.h" 
#include "Display.h"
/*    
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.��ʼ����ʱ��4������ң������
------------------------------------
*/



void TIM4_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  //��ʱ��
    
    TIM_DeInit(TIM4);

    TIM_TimeBaseStructure.TIM_Period = 1000;
    TIM_TimeBaseStructure.TIM_Prescaler = 36-1;//��ʱ1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    

   
    /* NVIC_PriorityGroup 1 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    //TIM4
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM4,ENABLE);
    
}		

int Ledcounter;


//��ʱ��������ں���
//��ʱ����Ϊ1ms
//���߼�����������ʾң���źŵ�ǿ�ȣ�ң���ϵ�����Խ�죬�ź�Խ��
//��Ϊ���ͺ��������и�����ط����������ղ���Ӧ���ź�ʱ���ڷ��ͺ��������ʱ���ط�����Խ��
//��ʱԽ�࣬LED�ͻ��������Խ����������

void TIM4_IRQHandler(void)		//1ms�ж�һ��
{
    if( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
    {     
     Ledcounter++;
      
     if(Ledcounter==59)Led4=1;
     if(Ledcounter==60){Led4=0;Ledcounter=0;}
     
      TxBuf[0]=0xAB;//��ʼ2�ֽ���֡ͷ
      TxBuf[1]=0xBA; 
      TxBuf[5]=(Throttle*10)&0xff;
      TxBuf[6]=((Throttle*10)>>8);     //������װ������������ǰ�����ֽ�
      TxBuf[2]=Pitch;                  //������װ���3���ֽ�
      TxBuf[3]=Roll;                   //�����ת���4���ֽ�
      TxBuf[4]=Yaw;                    //ƫ����װ���5���ֽ�
      TxBuf[31]=(Throttle<=5) ? 0:0xA5;//����Ϩ���־�ֽڿ���     
      NRF24L01_TxPacket(TxBuf);	//���ݰ����ͣ�����ط�����15��
     
     
      TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);   //����жϱ�־  
      
        
        
    } 
}

