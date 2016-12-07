/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef PCA10001_H
#define PCA10001_H

#define nRF_Serial_To_stm 

// #define LED_START      18
// #define LED0           18
// #define LED_STOP       19
// #define LED1           19
// #define LED_PORT       NRF_GPIO_PORT_SELECT_PORT2
// #define LED_OFFSET     2

// #define BUTTON_START   16
// #define BUTTON0        16
// #define BUTTON_STOP    17
// #define BUTTON1        17

#ifdef nRF_Serial_To_stm         //to STM32
    #define RX_PIN_NUMBER  28    // UART RX pin number.
    #define TX_PIN_NUMBER  29    // UART TX pin number.
#else                            //to PC
		#define RX_PIN_NUMBER  29    // UART RX pin number.
		#define TX_PIN_NUMBER  28    // UART TX pin number.
#endif

#define CTS_PIN_NUMBER 10    // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 8    // Not used if HWFC is set to false
#define HWFC           false // UART hardware flow control


#define BLINKY_STATE_MASK   0x01

#endif
