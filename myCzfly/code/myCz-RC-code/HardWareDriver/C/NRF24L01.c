#include "Config.h"

/*    
编译环境：MDK-Lite  Version: 4.23
初版时间: 2015-01-7
功能：
1.内部ADC初始化包括温度计、电位器
------------------------------------
*/
//

u8 TxBuf[32];//发送数组
u8 RxBuf[32];//接收数组

// 无线地址设置
//地址一共40个add_bit是add0~add39
//在2401地址add0先写(add0,add1,add2......add39)

//那2401和51822的地址关系如下
// 2401[add0 :add7] =PREFIX0[bit7:bit0](add7,add6,add5,add4,add3,add2,add1,add0)
// 2401[add8 :add15]=BASE0[bit31:bit24](add15,add14,add13,add12,add11,add10,add9,add8)
// 2401[add16:add23]=BASE0[bit23:bit16](add23,add22,add21,add20,add19,add18,add17,add16)
// 2401[add24:add31]=BASE0[bit15:bit8] (add31,add30,add29,add28,add27,add26,add25,add24)
// 2401[add32:add39]=BASE0[bit7 :bit0] (add39,add38,add37,add36,add35,add34,add33,add32)

//// 通道3 到 0 的低字节
//	NRF_RADIO->PREFIX0 =(0x23<<24) // 通道3的低字节地址
//	                   |(0xC3<<16) // 通道2的低字节地址
//	                   |(0x43<< 8) // 通道1的低字节地址
//	                   |(0xE7<< 0);// 通道0的低字节地址对应2401地址的[add7:add0]
//// 通道7 到 4 的低字节地址
//	NRF_RADIO->PREFIX1 =(0x13<<24) // 通道7的低字节地址
//	                   |(0xE3<<16) // 通道6的低字节地址
//	                   |(0x63<< 8) // 通道5的低字节地址
//	                   |(0xA3<< 0);// 通道4的低字节地址
// 
//  NRF_RADIO->BASE0   = (0xE7<<24)  // 通道0的高字节对应2401的[add15:add8]
//                      |(0xE7<<16)  //对应2401的[add23:add16]
//	                    |(0xE7<<8)   //对应2401的[add31:add24]
//	                    |(0xE7<<0);  //对应2401的[add39:add32]

//  NRF_RADIO->BASE1   = (0x43<<24)  // 通道1-7的高字节
//                      |(0x43<<16)
//	                    |(0x43<<8)
//	                    |(0x43<<0);
//以上结论得出的2401地址是每8个bit首尾对调。以下是官方用的地址
const u8  TX_ADDRESS[TX_ADR_WIDTH]={0xE7,0xE7,0xE7,0xE7,0xE7};//发射地址  从[bit0~7] [bit8~15] [bit16~23] [bit24~31] [bit32~39]
const u8 RX_ADDRESS0[RX_ADR_WIDTH]={0xE7,0xE7,0xE7,0xE7,0xE7};//接收地址0 和51822通讯我调了(bit0先写)
const u8 RX_ADDRESS1[RX_ADR_WIDTH]={0xC2,0xC2,0xC2,0xC2,0xC2};//接收地址1 bit39~bit8为"0x43"(bit0先写)
const u8 RX_ADDRESS2[RX_ADR_WIDTH]={0xC3,0x00};//接收地址2 bit39~8和通道1共用,只写bit0~bit7
const u8 RX_ADDRESS3[RX_ADR_WIDTH]={0xC4,0x00};//接收地址3	通道2345只能设置char
const u8 RX_ADDRESS4[RX_ADR_WIDTH]={0xC5,0x00};//接收地址4	写的时候顺序是bit0~bit39
const u8 RX_ADDRESS5[RX_ADR_WIDTH]={0xC6,0x00};//接收地址5		只有bit0~7有用。接收地址6：0xC7 接收地址7：0xC8

 
//初始化24L01的IO口
u8 NRF24L01_Init(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA口时钟 
	RCC->APB2ENR|=1<<3;    //使能PORTB口时钟 

	GPIOA->CRH&=0XFFF00FFF;//PA12-NRF-CE输出,A11-NRF-NRF_IRQ输入
	GPIOA->CRH|=0X00038000;
	
	GPIOB->CRH&=0XFFF0FFFF;//PB12-NRF-CSN输出
	GPIOB->CRH|=0X00030000; 
	
	
	NRF24L01_CE=1;
	NRF24L01_CSN=1; 
	SPI2_Init();    //初始化SPI
	//针对NRF的特点修改SPI的设置
 	SPI2->CR1&=~(1<<6); 	//SPI设备失能
	SPI2->CR1&=~(1<<1); 	//空闲模式下SCK为0 CPOL=0
	SPI2->CR1&=~(1<<0); 	//数据采样从第1个时间边沿开始,CPHA=0  
	SPI2->CR1|=1<<6; 		//SPI设备使能
  
	NRF24L01_CE=0; 	//使能24L01
	NRF24L01_CSN=1;	//SPI片选取消	
	return NRF24L01_Check();
}


//检测24L01是否存在
//返回值:0，成功;1，失败	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XF0,0XF0,0XF0,0XF0,0XF0};  //低位在前，高位在后
	u8 i;
	
	SPI2_SetSpeed(SPI_SPEED_16); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,buf,5);//写入5个字节的地址.	
	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XF0)break;	 							   
	if(i!=5)
	{
		printf("\r\n 检测nRF24L01错误！\n\r");
		return 1;//检测24L01错误	
	}
	printf("\r\n 检测nRF24L01成功！\n\r");
	return 0;		 //检测到24L01
}	 	 
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
   	NRF24L01_CSN=0;                 //使能SPI传输
  	status =SPI2_ReadWriteByte(reg);//发送寄存器号 
  	SPI2_ReadWriteByte(value);      //写入寄存器的值
  	NRF24L01_CSN=1;                 //禁止SPI传输	   
  	return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //使能SPI传输		
  	SPI2_ReadWriteByte(reg);   //发送寄存器号
  	reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
  	NRF24L01_CSN = 1;          //禁止SPI传输		    
  	return(reg_val);           //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  	NRF24L01_CSN = 0;           //使能SPI传输
  	status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
  	NRF24L01_CSN=1;       //关闭SPI传输
  	return status;        //返回读到的状态值
}



//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值



u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //使能SPI传输
  	status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
  	NRF24L01_CSN = 1;       //关闭SPI传输
  	return status;          //返回读到的状态值
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI2_SetSpeed(SPI_SPEED_8);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送	
	while(NRF24L01_IRQ!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_SPEED_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取32个字节数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
}					    

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void RX_Mode(void)
{
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除TX FIFO寄存器			 
	
	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_AA,0);    //应答设置0x3f代表543210应答   
	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_RXADDR,0x3f);//接收通道使能0x3f代表543210通道接收	 
	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_CH,24);	     //设置RF通信频率24MHz+2400MHz=2424MHz		  
	
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS0,RX_ADR_WIDTH);  //写RX0节点地址
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P1,(u8*)RX_ADDRESS1,RX_ADR_WIDTH);  //写RX1节点地址 
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P2,(u8*)RX_ADDRESS2,RX_ADR_WIDTH-4);//写RX2节点地址（只写1次，原因高位被锁了和通道1共用）
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P3,(u8*)RX_ADDRESS3,RX_ADR_WIDTH-4);//写RX3节点地址
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P4,(u8*)RX_ADDRESS4,RX_ADR_WIDTH-4);//写RX4节点地址
  NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P5,(u8*)RX_ADDRESS5,RX_ADR_WIDTH-4);//写RX节点地址

	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P1,RX_PLOAD_WIDTH);//选择通道1的有效数据宽度 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P2,RX_PLOAD_WIDTH);//选择通道2的有效数据宽度 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P3,RX_PLOAD_WIDTH);//选择通道3的有效数据宽度 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P4,RX_PLOAD_WIDTH);//选择通道4的有效数据宽度 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P5,RX_PLOAD_WIDTH);//选择通道5的有效数据宽度 	    
	
	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(WRITE_REG_NRF+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	NRF24L01_CE = 1; //CE为高,进入接收模式 
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void TX_Mode(void)
{														 
	  NRF24L01_CE=0;	
	  NRF24L01_Write_Reg(FLUSH_TX,0xff);										//清除TX FIFO寄存器		  
  	NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);		//写TX节点地址 
  	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS0,RX_ADR_WIDTH); 	//设置TX节点地址,主要为了使能ACK	  

  	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_AA,0);     //应答设置0x3f代表543210应答
  	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_RXADDR,0x3f); //使能通道543210的接收
  	NRF24L01_Write_Reg(WRITE_REG_NRF+SETUP_RETR,0x1E);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:15次
  	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_CH,24);       //设置RF通信频率24MHz+2400MHz=2424MHz		  
  	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(WRITE_REG_NRF+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
  	NRF24L01_CE=1;//CE为高,10us后启动发送
}		  

 


/****************************************************
函数名：Receive_2Byte_Data
输入：Address
输出：Data_int
功能：将缓冲数组内，两个连续地址的数据组合为一个16位数据
****************************************************/
u16 Receive_2Byte_Data(u8 Address)
{
      u16 Data_int;
      Data_int=RxBuf[Address+1];
      Data_int<<=8;
      Data_int+=RxBuf[Address];
      return Data_int;
}




