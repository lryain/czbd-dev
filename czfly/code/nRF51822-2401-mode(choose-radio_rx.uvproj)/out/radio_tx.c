//arm_startup_nrf51.s是启动文件，包含中断定义
//#include "nrf51.h"//包含51822各个寄存器结构地址。以上2文件就可以控制51822了。
//nrf51_bitfields.h包含各个寄存器设置值的作用
#include "project_nrf51822.c"//包含51822工程需要用到的头文件（晶振，延时，io脚什么的

#include <stdbool.h>//调用bool
#include "stdio.h"//标准输入输出
#include <stdint.h>//声明已知大小的整数或显示特征的整数
#include "simple_uart.h"

//packet[3]中QxC(Q开始，C结束，x是数据char)
static uint8_t packet[32];

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
    //接收用usb-jtag的nrf51822
//	  gpio_init_in(9,9,3);//p0.09输入
//	  gpio_init_in(11,11,3);//p0.11输入
//    gpio_init_out(21,23);//led:red,green,blue
	
    	
    	gpio_init_out(17,18);//设置18~25脚配置为输出/那个26~27接的是低速晶振

}





	int main(void)//main主程序开始
{
    init();//时钟初始化
    ioinit();//引脚初始化    
	  simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);

	  radio_configure();//无线电配置（用于和nrf2401通讯）
	  // 设置有效载荷指针
    NRF_RADIO->PACKETPTR = (uint32_t)packet;//指针指向数据缓冲packet  
	
	//检查io脚是否正常
    //NRF_GPIO->OUTSET=(1<<17);//led4亮，表示51822已经工作中
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
						serial_falg++;//前面19个数据发送到nRF51串口缓存[32]大小，剩余填充0，一直填满。。。
					}
        }
				if(serial_falg ==0)
				{
					//数据准备发送
					NRF_RADIO->EVENTS_READY = 0U;//收发模式切换完成标志位。复位
					NRF_RADIO->TASKS_TXEN = 1U;//启动无线电为发射模式
							while (NRF_RADIO->EVENTS_READY == 0U);//等待收发模式切换完成
							// 启动数据传输
					NRF_RADIO->EVENTS_END  = 0U;//传输结束标志位，复位    
					NRF_RADIO->TASKS_START = 1U;//开始传输		
					while(NRF_RADIO->EVENTS_END == 0U) //等待传输结束
					{ 					
							nrf_gpio_pin_set(18);//传输等待led0亮
					}
					nrf_gpio_pin_clear(18);//led0灭，表示数据已经发出
					// 组1引脚（8~15）
					//nrf_gpio_port_write(P1, packet[1]);//数值显示在单片机P1口上
					
					
					NRF_RADIO->EVENTS_DISABLED = 0U;//传输关闭标志位  复位
					NRF_RADIO->TASKS_DISABLE   = 1U; // 关闭传输
					while(NRF_RADIO->EVENTS_DISABLED == 0U)//等待传输关闭
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
