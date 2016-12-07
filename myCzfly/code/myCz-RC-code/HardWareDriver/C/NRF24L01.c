#include "Config.h"

/*    
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2015-01-7
���ܣ�
1.�ڲ�ADC��ʼ�������¶ȼơ���λ��
------------------------------------
*/
//

u8 TxBuf[32];//��������
u8 RxBuf[32];//��������

// ���ߵ�ַ����
//��ַһ��40��add_bit��add0~add39
//��2401��ַadd0��д(add0,add1,add2......add39)

//��2401��51822�ĵ�ַ��ϵ����
// 2401[add0 :add7] =PREFIX0[bit7:bit0](add7,add6,add5,add4,add3,add2,add1,add0)
// 2401[add8 :add15]=BASE0[bit31:bit24](add15,add14,add13,add12,add11,add10,add9,add8)
// 2401[add16:add23]=BASE0[bit23:bit16](add23,add22,add21,add20,add19,add18,add17,add16)
// 2401[add24:add31]=BASE0[bit15:bit8] (add31,add30,add29,add28,add27,add26,add25,add24)
// 2401[add32:add39]=BASE0[bit7 :bit0] (add39,add38,add37,add36,add35,add34,add33,add32)

//// ͨ��3 �� 0 �ĵ��ֽ�
//	NRF_RADIO->PREFIX0 =(0x23<<24) // ͨ��3�ĵ��ֽڵ�ַ
//	                   |(0xC3<<16) // ͨ��2�ĵ��ֽڵ�ַ
//	                   |(0x43<< 8) // ͨ��1�ĵ��ֽڵ�ַ
//	                   |(0xE7<< 0);// ͨ��0�ĵ��ֽڵ�ַ��Ӧ2401��ַ��[add7:add0]
//// ͨ��7 �� 4 �ĵ��ֽڵ�ַ
//	NRF_RADIO->PREFIX1 =(0x13<<24) // ͨ��7�ĵ��ֽڵ�ַ
//	                   |(0xE3<<16) // ͨ��6�ĵ��ֽڵ�ַ
//	                   |(0x63<< 8) // ͨ��5�ĵ��ֽڵ�ַ
//	                   |(0xA3<< 0);// ͨ��4�ĵ��ֽڵ�ַ
// 
//  NRF_RADIO->BASE0   = (0xE7<<24)  // ͨ��0�ĸ��ֽڶ�Ӧ2401��[add15:add8]
//                      |(0xE7<<16)  //��Ӧ2401��[add23:add16]
//	                    |(0xE7<<8)   //��Ӧ2401��[add31:add24]
//	                    |(0xE7<<0);  //��Ӧ2401��[add39:add32]

//  NRF_RADIO->BASE1   = (0x43<<24)  // ͨ��1-7�ĸ��ֽ�
//                      |(0x43<<16)
//	                    |(0x43<<8)
//	                    |(0x43<<0);
//���Ͻ��۵ó���2401��ַ��ÿ8��bit��β�Ե��������ǹٷ��õĵ�ַ
const u8  TX_ADDRESS[TX_ADR_WIDTH]={0xE7,0xE7,0xE7,0xE7,0xE7};//�����ַ  ��[bit0~7] [bit8~15] [bit16~23] [bit24~31] [bit32~39]
const u8 RX_ADDRESS0[RX_ADR_WIDTH]={0xE7,0xE7,0xE7,0xE7,0xE7};//���յ�ַ0 ��51822ͨѶ�ҵ���(bit0��д)
const u8 RX_ADDRESS1[RX_ADR_WIDTH]={0xC2,0xC2,0xC2,0xC2,0xC2};//���յ�ַ1 bit39~bit8Ϊ"0x43"(bit0��д)
const u8 RX_ADDRESS2[RX_ADR_WIDTH]={0xC3,0x00};//���յ�ַ2 bit39~8��ͨ��1����,ֻдbit0~bit7
const u8 RX_ADDRESS3[RX_ADR_WIDTH]={0xC4,0x00};//���յ�ַ3	ͨ��2345ֻ������char
const u8 RX_ADDRESS4[RX_ADR_WIDTH]={0xC5,0x00};//���յ�ַ4	д��ʱ��˳����bit0~bit39
const u8 RX_ADDRESS5[RX_ADR_WIDTH]={0xC6,0x00};//���յ�ַ5		ֻ��bit0~7���á����յ�ַ6��0xC7 ���յ�ַ7��0xC8

 
//��ʼ��24L01��IO��
u8 NRF24L01_Init(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTA��ʱ�� 
	RCC->APB2ENR|=1<<3;    //ʹ��PORTB��ʱ�� 

	GPIOA->CRH&=0XFFF00FFF;//PA12-NRF-CE���,A11-NRF-NRF_IRQ����
	GPIOA->CRH|=0X00038000;
	
	GPIOB->CRH&=0XFFF0FFFF;//PB12-NRF-CSN���
	GPIOB->CRH|=0X00030000; 
	
	
	NRF24L01_CE=1;
	NRF24L01_CSN=1; 
	SPI2_Init();    //��ʼ��SPI
	//���NRF���ص��޸�SPI������
 	SPI2->CR1&=~(1<<6); 	//SPI�豸ʧ��
	SPI2->CR1&=~(1<<1); 	//����ģʽ��SCKΪ0 CPOL=0
	SPI2->CR1&=~(1<<0); 	//���ݲ����ӵ�1��ʱ����ؿ�ʼ,CPHA=0  
	SPI2->CR1|=1<<6; 		//SPI�豸ʹ��
  
	NRF24L01_CE=0; 	//ʹ��24L01
	NRF24L01_CSN=1;	//SPIƬѡȡ��	
	return NRF24L01_Check();
}


//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XF0,0XF0,0XF0,0XF0,0XF0};  //��λ��ǰ����λ�ں�
	u8 i;
	
	SPI2_SetSpeed(SPI_SPEED_16); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XF0)break;	 							   
	if(i!=5)
	{
		printf("\r\n ���nRF24L01����\n\r");
		return 1;//���24L01����	
	}
	printf("\r\n ���nRF24L01�ɹ���\n\r");
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
   	NRF24L01_CSN=0;                 //ʹ��SPI����
  	status =SPI2_ReadWriteByte(reg);//���ͼĴ����� 
  	SPI2_ReadWriteByte(value);      //д��Ĵ�����ֵ
  	NRF24L01_CSN=1;                 //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����		
  	SPI2_ReadWriteByte(reg);   //���ͼĴ�����
  	reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
  	NRF24L01_CSN = 1;          //��ֹSPI����		    
  	return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  	NRF24L01_CSN = 0;           //ʹ��SPI����
  	status=SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
  	NRF24L01_CSN=1;       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}



//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ



u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //ʹ��SPI����
  	status = SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
  	NRF24L01_CSN = 1;       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI2_SetSpeed(SPI_SPEED_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE=0;
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE=1;//��������	
	while(NRF24L01_IRQ!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ���
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ32���ֽ�����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void RX_Mode(void)
{
	NRF24L01_CE=0;
	NRF24L01_Write_Reg(FLUSH_RX,0xff);//���TX FIFO�Ĵ���			 
	
	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_AA,0);    //Ӧ������0x3f����543210Ӧ��   
	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_RXADDR,0x3f);//����ͨ��ʹ��0x3f����543210ͨ������	 
	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_CH,24);	     //����RFͨ��Ƶ��24MHz+2400MHz=2424MHz		  
	
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS0,RX_ADR_WIDTH);  //дRX0�ڵ��ַ
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P1,(u8*)RX_ADDRESS1,RX_ADR_WIDTH);  //дRX1�ڵ��ַ 
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P2,(u8*)RX_ADDRESS2,RX_ADR_WIDTH-4);//дRX2�ڵ��ַ��ֻд1�Σ�ԭ���λ�����˺�ͨ��1���ã�
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P3,(u8*)RX_ADDRESS3,RX_ADR_WIDTH-4);//дRX3�ڵ��ַ
	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P4,(u8*)RX_ADDRESS4,RX_ADR_WIDTH-4);//дRX4�ڵ��ַ
  NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P5,(u8*)RX_ADDRESS5,RX_ADR_WIDTH-4);//дRX�ڵ��ַ

	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P1,RX_PLOAD_WIDTH);//ѡ��ͨ��1����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P2,RX_PLOAD_WIDTH);//ѡ��ͨ��2����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P3,RX_PLOAD_WIDTH);//ѡ��ͨ��3����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P4,RX_PLOAD_WIDTH);//ѡ��ͨ��4����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(WRITE_REG_NRF+RX_PW_P5,RX_PLOAD_WIDTH);//ѡ��ͨ��5����Ч���ݿ�� 	    
	
	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_Write_Reg(WRITE_REG_NRF+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	NRF24L01_CE = 1; //CEΪ��,�������ģʽ 
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void TX_Mode(void)
{														 
	  NRF24L01_CE=0;	
	  NRF24L01_Write_Reg(FLUSH_TX,0xff);										//���TX FIFO�Ĵ���		  
  	NRF24L01_Write_Buf(WRITE_REG_NRF+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);		//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,(u8*)RX_ADDRESS0,RX_ADR_WIDTH); 	//����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_AA,0);     //Ӧ������0x3f����543210Ӧ��
  	NRF24L01_Write_Reg(WRITE_REG_NRF+EN_RXADDR,0x3f); //ʹ��ͨ��543210�Ľ���
  	NRF24L01_Write_Reg(WRITE_REG_NRF+SETUP_RETR,0x1E);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:15��
  	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_CH,24);       //����RFͨ��Ƶ��24MHz+2400MHz=2424MHz		  
  	NRF24L01_Write_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(WRITE_REG_NRF+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
  	NRF24L01_CE=1;//CEΪ��,10us����������
}		  

 


/****************************************************
��������Receive_2Byte_Data
���룺Address
�����Data_int
���ܣ������������ڣ�����������ַ���������Ϊһ��16λ����
****************************************************/
u16 Receive_2Byte_Data(u8 Address)
{
      u16 Data_int;
      Data_int=RxBuf[Address+1];
      Data_int<<=8;
      Data_int+=RxBuf[Address];
      return Data_int;
}




