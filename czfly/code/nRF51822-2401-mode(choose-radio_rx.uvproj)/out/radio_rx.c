//arm_startup_nrf51.s是启动文件，包含中断定义
//#include "nrf51.h"//包含51822各个寄存器结构地址。以上2文件就可以控制51822了。
//nrf51_bitfields.h包含各个寄存器设置值的作用
#include "project_nrf51822.c"//包含51822工程需要用到的头文件（晶振，延时，io脚什么的

#include <stdbool.h>//调用bool
#include "stdio.h"//标准输入输出
#include <stdint.h>//声明已知大小的整数或显示特征的整数


void init(void)//51822时钟初始化
{ 
	 //判断电源时钟状态
	 if ((((*(uint32_t *)0xF0000FE0) & 0xFF) == 1) && (((*(uint32_t *)0xF0000FE4) & 0xF) == 0))
    {
        if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x0) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
					//解决radio无法写入  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//电源时钟设置
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE设置    
        }
				else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x10) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //解决radio无法写入  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//电源时钟设置
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE设置  
        }
        else if ((((*(uint32_t *)0xF0000FE8) & 0xF0) == 0x30) && (((*(uint32_t *)0xF0000FEC) & 0xF0) == 0x0))
        {
          //解决radio无法写入  
          *(uint32_t *)0x40000504 = 0xC007FFDF;//电源时钟设置
          *(uint32_t *)0x40006C18 = 0x00008000;//GPIOTE设置    
        }
    }
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;//HFCLK晶振运行状态0关闭，1运行
  NRF_CLOCK->TASKS_HFCLKSTART = 1;//高外晶振HFCLK 0关闭，1启动
  while(!NRF_CLOCK->EVENTS_HFCLKSTARTED);//等待HFCLK晶振运行状态为1
}

void ioinit()//51822引脚初始化
{
   gpio_init_out(17,18);//设置0~15脚配置为输出
}


//packet[3]中QxC(Q开始，C结束，x是数据char)
static uint8_t volatile packet[32];  //< 数据传输的缓冲包packet[1]是数据

int main(void)//main主程序开始
{
    int i;
	  uint32_t  RF_timeOver_flag;
    float vbatLi;//电池相关的变量声明
	  PMStates pmStateOld = battery,pmState; //电池充电管理相关的变量声明 
	  init();//时钟初始化
    ioinit();//引脚初始化
	  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
	  radio_configure();//无线电配置（也可用于和nrf2401通讯）
	  nrf_adc_init_vbat();
	  pmInit();
	  pmSetChargeState(charge500mA);
 
	  // 设置有效载荷指针
    NRF_RADIO->PACKETPTR    = (uint32_t)packet;//指针指向数据缓冲packet
    nrf_gpio_pin_clear(17);
	  nrf_gpio_pin_clear(18);
    while(1)
    {
			  pmState=pmUpdateState();
			  //vbatLi=nrf_adc_read_vbat_f(); 
			  NRF_RADIO->EVENTS_READY = 0U; //收发模式转换完成  标志位       
        NRF_RADIO->TASKS_RXEN   = 1U; //接收模式
        while(NRF_RADIO->EVENTS_READY == 0U) //等待收发模式转换完成(接收模式)标志位
          {
            // Do nothing.等待
          }
        NRF_RADIO->EVENTS_END  = 0U;//传输完成  标志位     
        NRF_RADIO->TASKS_START = 1U; // 开始传输

        while(NRF_RADIO->EVENTS_END == 0U)//等待传输完成  标志位
           {
						 // Do nothing.等待
//              RF_timeOver_flag++;
// 						 if(RF_timeOver_flag>500000)break;//如果长时间没有接收到无线数据包，跳出while循环。。。不至于影响其他程序运行！！！
           }
        if (NRF_RADIO->CRCSTATUS == 1U)//如果CRC校验正确
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

        NRF_RADIO->EVENTS_DISABLED = 0U;//无线关闭   标志位
        NRF_RADIO->TASKS_DISABLE   = 1U;// 关闭无线设备

        while(NRF_RADIO->EVENTS_DISABLED == 0U)//等待设备关闭
        {
            // Do nothing.
        }
    }   
}

