/**********************************************************************
 * Name: Uart Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/23/2026 (mm/dd/yyyy)
 * Creation Date: 10/22/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    *  Used to send serial uart between the board and a PC
**********************************************************************/


#include "uart_driver.h"

static void uart3_send_char(char c) {
    while (!(USART3->SR & USART_SR_TXE)); // Wait until TXE (Transmit Data Register Empty) flag is set
    USART3->DR = c; // Write the character to the data register
}

void serial_printf(char *string, int length) {
    for (int i=0; i<length; i++){
        if (string[i] != '\0') {
            uart3_send_char(string[i]);
        }
        else{
            break;
        }
    }
}