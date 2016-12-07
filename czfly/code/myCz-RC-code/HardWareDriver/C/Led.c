#include "Led.h"


/*    
      ____                      _____                  +---+
     / ___\                     / __ \                 | R |
    / /                        / /_/ /                 +---+
   / /   ________  ____  ___  / ____/___  ____  __   __
  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /
 / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /
 \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /
                                                 / /
                                            ____/ /
                                           /_____/
main.c file
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-03-28
���ܣ�
1.����״ָ̬ʾ�Ƴ�ʼ��
------------------------------------
*/



/********************************************
              Led��ʼ������
���ܣ�
1.����Led�ӿ�IO�������
2.�ر�����Led(����Ĭ�Ϸ�ʽ)

������
Led�ӿڣ�
Led1-->PB0
Led2-->PB1
Led3-->PB3
Led4-->PB11
��ӦIO=1������
********************************************/
void LedInit(void)
{
    RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��
  	RCC->APB2ENR|=1<<0;    //ʹ�ܸ���ʱ��	   
	
    GPIOB->CRL&=0XFFFF0F00;  //PB0,1,3�������
    GPIOB->CRL|=0X00003033;
	  
	  GPIOB->CRH&=0XFFFF0FFF;  //PB11�������
    GPIOB->CRH|=0X00003000;
    //1000 0000 1011
	  GPIOB->ODR|=0x80B;        //PB0,1,3,11����
    AFIO->MAPR|=2<<24;      //�ر�JATG,ǧ���ܽ�SWDҲ�رգ�����оƬ���ϣ��ײ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //PB3 Ĭ����JTDO�ӿ�,���ڵ��Խӿڣ����ù���ΪPB4 IO��
   Led1=0;
   Led2=0;
   Led3=0;
   Led4=0;
}


