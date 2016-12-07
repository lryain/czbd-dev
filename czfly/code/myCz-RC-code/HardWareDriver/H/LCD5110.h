#ifndef __LCD5110_H__
#define __LCD5110_H__

#include "stm32f10x.h"

/*GPIO_PIN�ڶ���*/
#define SCLK_PIN GPIO_Pin_3
#define SDIN_PIN GPIO_Pin_4
#define DC_PIN   GPIO_Pin_5
#define CE_PIN   GPIO_Pin_6
#define RST_PIN  GPIO_Pin_7


/*GPIO�鶨��*/
#define RCC_APB2Periph_LCD_PORT   RCC_APB2Periph_GPIOB
#define LCD_GPIO GPIOB


/*IO��ƽ�趨�궨��*/
#define	SCLK1 GPIO_SetBits(LCD_GPIO, SCLK_PIN); 
#define	SCLK0 GPIO_ResetBits(LCD_GPIO, SCLK_PIN); 
#define	SDIN1 GPIO_SetBits(LCD_GPIO, SDIN_PIN); 
#define	SDIN0 GPIO_ResetBits(LCD_GPIO, SDIN_PIN);
#define	DC1 GPIO_SetBits(LCD_GPIO, DC_PIN); 
#define	DC0 GPIO_ResetBits(LCD_GPIO, DC_PIN);
#define	CE1 GPIO_SetBits(LCD_GPIO, CE_PIN); 
#define	CE0 GPIO_ResetBits(LCD_GPIO, CE_PIN);
#define	RST1 GPIO_SetBits(LCD_GPIO, RST_PIN); 
#define	RST0 GPIO_ResetBits(LCD_GPIO, RST_PIN);

/*�ɹ��ⲿ���õĺ�������*/

void LCD_Write_Byte(u8 dat,u8 com);	/*datΪ��5110��д��һ���ֽڵ����ݣ�
				       	  ��com����1���ʾд��������ݣ�
				       	  ��com����0���ʾд���������*/

void LCD_Clear(void);			/*ȫ���������*/

void LCD_Init(void);			/*Һ����ʼ������*/

void LCD_Set_XY(u8 X,u8 Y);		/*�趨��һ��ʾλ�õ��������꣬�к�
					  �о���0��ʼ*/

void LCD_Write_Char(u8 c);		/*��Һ���ϵ�ǰ���λ�����һ���ַ�*/


/*��Һ���ϵ�(X,Y)���괦��ʾһ���ַ���*/
void LCD_write_english_string(u8 X,u8 Y,char *s);
					
void LCD_On(void);/*Һ������*/

void LCD_Off(void);/*�ر�Һ��*/



#endif


