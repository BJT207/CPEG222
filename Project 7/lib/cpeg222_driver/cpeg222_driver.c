/************************************************************************
 * Name: CPEG222 Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/29/2026 (mm/dd/yyyy)
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
