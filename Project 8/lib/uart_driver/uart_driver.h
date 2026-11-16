/**********************************************************************
 * Name: Uart Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/22/2026 (mm/dd/yyyy)
 * Creation Date: 10/22/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    *  Used to send serial uart between the board and a PC
**********************************************************************/
#ifndef uart_driver
#define uart_driver
    // Pins
        #define UART_TX_PIN 8
        #define UART_RX_PIN 9
        #define UART_GPIO GPIOD
    
    // Function Headers
        void serial_printf(char *, int); // Lets you print a line to a connected computer's serial terminal

    // Inline Macros
    #include "stm32f446xx.h"
    static inline void ENABLE_UART(void) {
        // Enable GPIOD and USART3 clocks
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        // Set PD8 (TX) and PD9 (RX) to alternate function AF7
        UART_GPIO->MODER &= ~((0x3 << (UART_TX_PIN * 2)) | (0x3 << (UART_RX_PIN * 2)));
        UART_GPIO->MODER |= (0x2 << (UART_TX_PIN * 2)) | (0x2 << (UART_RX_PIN * 2));
        UART_GPIO->AFR[1] &= ~((0xF << ((UART_TX_PIN - 8) * 4)) | (0xF << ((UART_RX_PIN - 8) * 4)));
        UART_GPIO->AFR[1] |= (0x7 << ((UART_TX_PIN - 8) * 4)) | (0x7 << ((UART_RX_PIN - 8) * 4));
        // Configure USART3: 115200 baud, 8N1 (oversampling by 16)
        USART3->BRR = 390;
        USART3->CR1 = USART_CR1_TE | USART_CR1_UE; // Enable TX, RX, USART
    }
#endif