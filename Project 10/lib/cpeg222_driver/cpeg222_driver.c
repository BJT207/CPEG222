/************************************************************************
 * Name: CPEG222 Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/4/2026 (mm/dd/yyyy)
 * Creation Date: 10/21/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
    * CPEG222 Robot Base Rev.C
 * Description:
    * Pins, startup macros, and common functions used for CPEG222 @ UDEL
 * Attribution:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
************************************************************************/
#include "cpeg222_driver.h"

// Constants
    const unsigned char ssddigitSegments[] = {
            0b0111111, // 0
            0b0000110, // 1
            0b1011011, // 2
            0b1001111, // 3
            0b1100110, // 4
            0b1101101, // 5
            0b1111101, // 6
            0b0000111, // 7
            0b1111111, // 8
            0b1101111, // 9
            0b0000000  // blank/off
        };

    const uint16_t place[] = {
            1,
            10,
            100,
            1000
        };

    void delay_ms(uint32_t ms){
        SystemCoreClockUpdate();
        if(SystemCoreClock == 16000000U){ // If the system clock is 16 MHz
            for(uint32_t i = 0; i < ms * 2000; i++){
                __NOP();
            }
        }
        else if(SystemCoreClock == 180000000U){ // If the system clock is 180 MHz
            for(uint32_t i = 0; i < ms * 22500; i++){
                __NOP();
            }
        }
    }

    void ssd_display_digit(uint8_t digit, uint8_t number, bool dec_pnt) {
        /******************************************
         * SSD Display Digit
         * Description:
         *      Used to display digits 0-9 on the
         *      Trainer Board hat
        ******************************************/
        // Set all the digits to high (off) to prevent ghosting
        GPIOE->BSRR = (1U << SSD_DIGIT1_SEL) | (1U << SSD_DIGIT2_SEL);
        GPIOB->BSRR = (1U << SSD_DIGIT3_SEL) | (1U << SSD_DIGIT4_SEL);
        
        // Grab the sequence based on the number being displayed
        uint8_t segment_bits = ssddigitSegments[number];

        // Set the segmets to low bases on what needs to be displayed
        GPIOB->BSRR = (1U << (SSD_G + (((segment_bits >> 6) & 1U) ? 16U : 0)));
        GPIOE->BSRR = (1U << (SSD_E + (((segment_bits >> 4) & 1U) ? 16U : 0)));
        GPIOF->BSRR = (1U << (SSD_B + (((segment_bits >> 1) & 1U) ? 16U : 0))) | 
                    (1U << (SSD_C + (((segment_bits >> 2) & 1U) ? 16U : 0))) | 
                    (1U << (SSD_F + (((segment_bits >> 5) & 1U) ? 16U : 0))) | 
                    (1U << (SSD_DEC_PNT + ((dec_pnt==true) ? 16U : 0)));
        GPIOG->BSRR = (1U << (SSD_A + ((segment_bits & 1U) ? 16U : 0))) | 
                    (1U << (SSD_D + (((segment_bits >> 3) & 1U) ? 16U : 0)));


        // Set the digit that needs to be enabled back to low (on)
        switch (digit)
        {
        case 3: GPIOE->BSRR = (1U << (SSD_DIGIT1_SEL + 16U)); break;
        case 2: GPIOE->BSRR = (1U << (SSD_DIGIT2_SEL + 16U)); break;
        case 1: GPIOB->BSRR = (1U << (SSD_DIGIT3_SEL + 16U)); break;
        case 0: GPIOB->BSRR = (1U << (SSD_DIGIT4_SEL + 16U)); break;
        }        
    }

    void hat_led_display_digits(uint8_t lights) {
        GPIOD->BSRR |= lights | (0xFFFF << 16);
    }