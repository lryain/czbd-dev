#include "Key.h"
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
Tim.c file
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.����IO�ڳ�ʼ��
2.�ⲿ�ְ����Ĺ����Ƕ��⿪�ŵģ���û�ж��� �书�ܣ�������ɷ��Ӱ�
------------------------------------
*/
/********************************************
              Key��ʼ������
���ܣ�
1.����Key�ӿ�IO���뷽��

������
Key�ӿڣ�
Key1-->PB8
Key2-->PB10
Key3-->PB9
********************************************/
void KeyInit(void)
{
    RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��
    GPIOB->CRL&=0XFFFFF000;  //PB8,9,10����
    GPIOB->CRL|=0X00000888;
    GPIOB->ODR|=7<<8;        //PB8,9,10����
 
}


