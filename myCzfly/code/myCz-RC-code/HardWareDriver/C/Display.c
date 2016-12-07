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
编写者：小马  (Camel)
作者E-mail：375836945@qq.com
编译环境：MDK-Lite  Version: 4.23
初版时间: 2014-01-28
功能：
1.这部分代码，是最初几个版本用显示各类参数时用的，那时用的是诺基亚5110的屏幕，这部分代码是可以用的，而且很好用
2.大家可以去研究，5110驱动代码在源文件里面，没有加入该工程而已，所以在左边的工程文件看不到它
------------------------------------
*/

/*****************************************************************
 * 文件名   ：Display.c
 * 描述     : 屏幕显示驱动
 * 实验平台 ：STM32f103T8U6 
 * 库版本   ：ST3.5.0
 * 作者     ：MJ
 * 时间     ：2014-2-18 0:47:33
 * 情况     ：可以使用
 * 其它     ：今天查到考研成绩，哎  含着悲愤写着程序
 ******************************************************************/

 
#include "adc.h"
#include "usart.h"

int Throttle;
int Roll;
int Pitch;
int Yaw;
int Battery;//定义待显示量    
int PotentioL,PotentioR;
/*
摇杆数据采集
输入参数为美国手和日本手
*/
//日本手：左手方向舵，升降舵；右手副翼，油门舵
//美国手：左手方向舵，油门舵；右手副翼，升降舵
//中国手：左手副翼，升降舵；右手方向舵，油门舵
/******************日本手*********************
       Pitch                Throttle
         +                     +
         |                     |
     +---|---- Yaw         ----|---+ RoLL    
         |                     |
         |                     |
**********************************************/
/******************美国手*********************
      Throttle               Pitch
         +                     +
         |                     |
     +---|---- Yaw         ----|---+ RoLL    
         |                     |
         |                     |
**********************************************/
/******************中国手*********************
       Pitch                Throttle
         +                     +
         |                     |
     +---|---- Roll        ----|---+ Yaw    
         |                     |
         |                     |
**********************************************/

void GetAD(unsigned char RomoteMode)
{

  switch(RomoteMode)
  {
    /*中国手*/
 case China: 
	    Throttle=99-(100*Get_Adc_Average(1,15))/4096;//采集油门摇杆的位置，由于硬件原因，需要用100-采集值
      Throttle=(Throttle<=0)?0:Throttle;                 //越界判断
      Throttle=(Throttle>=99)?99:Throttle;               //越界判断

      Pitch=-1+(100*Get_Adc_Average(3,15))/4096;//采集俯仰摇杆的位置，赋值给对应的俯仰变量
      Pitch=(Pitch<=0)?0:Pitch;                 //越界判断
      Pitch=(Pitch>=99)?99:Pitch;               //越界判断

      Roll=1+(100*Get_Adc_Average(2,15))/4096;//采集横滚摇杆位置，赋值给对应的横滚变量
      Roll=(Roll<=0)?0:Roll;                //越界判断
      Roll=(Roll>=99)?99:Roll;              //越界判断

      Yaw=(100*Get_Adc_Average(0,15))/4096;//采集横滚摇杆位置，赋值给对应的偏航角
      Yaw=(Yaw<=0)?0:Yaw;                //越界判断
      Yaw=(Yaw>=99)?99:Yaw;              //越界判断

      Battery=(100*Get_Adc_Average(6,15))/2600;//采集遥控电池电压，赋值飞对应的电池变量
      Battery=(Battery>=99)?99:Battery;               //越界判断
   
      break;
		/*以下为美国手的对应关系*/
 case America:
                
      Throttle=99-(100*Get_Adc_Average(3,15))/4096;//采集油门摇杆的位置，由于硬件原因，需要用100-采集值
      Throttle=(Throttle<=0)?0:Throttle;                 //越界判断
      Throttle=(Throttle>=99)?99:Throttle;               //越界判断

      Pitch=-1+(100*Get_Adc_Average(1,15))/4096;//采集俯仰摇杆的位置，赋值给对应的俯仰变量
      Pitch=(Pitch<=0)?0:Pitch;                 //越界判断
      Pitch=(Pitch>=99)?99:Pitch;               //越界判断

      Roll=1+(100*Get_Adc_Average(0,15))/4096;//采集横滚摇杆位置，赋值给对应的横滚变量
      Roll=(Roll<=0)?0:Roll;                //越界判断
      Roll=(Roll>=99)?99:Roll;              //越界判断

      Yaw=(100*Get_Adc_Average(2,15))/4096;//采集横滚摇杆位置，赋值给对应的偏航角
      Yaw=(Yaw<=0)?0:Yaw;                //越界判断
      Yaw=(Yaw>=99)?99:Yaw;              //越界判断

      Battery=(100*Get_Adc_Average(6,15))/2600;//采集遥控电池电压，赋值飞对应的电池变量
      Battery=(Battery>=99)?99:Battery;               //越界判断
   
          break;
    /*以下为日本手的对应关系*/
 case Japan:
      Throttle=(100*Get_Adc_Average(1,15))/4096;//采集油门摇杆的位置，由于硬件原因，需要用100-采集值
      Throttle=(Throttle<=0)?0:Throttle;                 //越界判断
      Throttle=(Throttle>=99)?99:Throttle;               //越界判断

      Pitch=100-(100*Get_Adc_Average(3,15))/4096;//采集俯仰摇杆的位置，赋值给对应的俯仰变量
      Pitch=(Pitch<=0)?0:Pitch;                 //越界判断
      Pitch=(Pitch>=99)?99:Pitch;               //越界判断

      Roll=1+(100*Get_Adc_Average(0,15))/4096;//采集横滚摇杆位置，赋值给对应的横滚变量
      Roll=(Roll<=0)?0:Roll;                //越界判断
      Roll=(Roll>=99)?99:Roll;              //越界判断

      Yaw=(100*Get_Adc_Average(2,15))/4096;//采集横滚摇杆位置，赋值给对应的偏航角
      Yaw=(Yaw<=0)?0:Yaw;                //越界判断
      Yaw=(Yaw>=99)?99:Yaw;              //越界判断

      Battery=(100*Get_Adc_Average(6,15))/2600;//采集遥控电池电压，赋值飞对应的电池变量
      Battery=(Battery>=99)?99:Battery;               //越界判断
  
          break;
    
    
    
  }
// 	PotentioL=(100*Get_Adc_Average(5,15))/4096;//左电位器
// 	PotentioL=(PotentioL<=0)?0:PotentioL;                //越界判断
// 	PotentioL=(PotentioL>=99)?99:PotentioL;              //越界判断
// 	PotentioR=(100*Get_Adc_Average(4,15))/4096;//左电位器
// 	PotentioR=(PotentioR<=0)?0:PotentioR;                //越界判断
// 	PotentioR=(PotentioR>=99)?99:PotentioR;              //越界判断
//   printf("\r\nThrottle=%d Pitch=%d Roll=%d Yaw=%d Battery=%d PotentioL=%d  PotentioR=%d\r\n",Throttle,Pitch,Roll,Yaw,Battery,PotentioL,PotentioR);

}





