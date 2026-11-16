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
#include <stdarg.h>
#include <stdio.h>

static void uart3_send_char(char c) {
    while (!(USART3->SR & USART_SR_TXE)); // Wait until TXE (Transmit Data Register Empty) flag is set
    USART3->DR = c; // Write the character to the data register
}

void serial_printf(char *string, ...) {
    va_list args;
    va_start(args, string);
    for (const char *p = string; *p != '\0'; p++) {
        if (*p == '%' && *(p + 1) != '\0') {
            p++; // Move to the specifier character
            switch (*p) {
                case 'd': { // Integer
                    int i = va_arg(args, int);
                    char int_str[12] = "";
                    snprintf(int_str, sizeof(int_str), "%d", i);
                    for (char *int_ptr = int_str; *int_ptr != '\0'; int_ptr++){
                        uart3_send_char(*int_ptr);
                    }
                    break;
                }
                case 's': { // String
                    char *s = va_arg(args, char *);
                    for (char *str_pntr = s; *s != '\0'; str_pntr++) {
                        uart3_send_char(*s);
                    }
                    break;
                }
                case '.': { // Float (promoted to double in va_arg)
                    double f = va_arg(args, double);
                    printf("%.f", f);
                    break;
                }
                default:
                    putchar('%');
                    putchar(*p);
            }
        }
        else if (*p == '\0') {
            break;
        }
        else {
            uart3_send_char(*p);
        }
        
    }
}