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
                    for (char *str_pntr = s; *str_pntr != '\0'; str_pntr++) {
                        uart3_send_char(*str_pntr);
                    }
                    break;
                }
                case '.': { // Float (promoted to double in va_arg)
                    int places = 0;
                    int i = 1;
                    double f = va_arg(args, double);
                    for (;; i++){
                        if (*(p + i) >= '0' && *(p + i) <= '9') {
                            places = (places * 10) + (*(p + i) - '0');
                        }
                        else if (*(p+i) == 'f') {
                            char float_str[64] = {0};

                            snprintf(float_str, sizeof(float_str), "%.*f", places, f);
                            for (int j = 0; float_str[j] != '\0' && j < 64; j++ ) {
                                uart3_send_char(float_str[j]);
                            }
                        }
                        else {
                                // print everything between
                        }
                            p = p + i;
                            break;
                    }
                    break;
                }
                case 'f': {// Float (promoted to double in va_arg)
                    char float_str[64] = {0};
                    double f = va_arg(args, double);
                    snprintf(float_str, sizeof(float_str), "%f", f);
                    for (int j = 0; float_str[j] != '\0' && j < 64; j++ ) {
                        uart3_send_char(float_str[j]);
                    }
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