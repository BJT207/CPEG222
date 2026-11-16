/****************************************************************
 * CPEG222 Demo Project 1, 9/2/26
 * Author: B. Trainor
 * Written for the NucleoF455ZE CMIS STM32F4xx
 * Based on:
 *      CPEG222 DemoProgram1, 8/16/26
 *      Author: T. LUM and R. MARTIN
 *      NucleoF466ZE CMSIS STM32F4xx example Demo1 Blink PB0 (LD1)
****************************************************************/
/**
 * Include Headers
 */

#include "stm32f4xx.h" // Include the STM32F4xx CMSIS header file

#define BTN_PIN (13) // Assuming User Button is connected to GPIOC pin 13
#define BTN_PORT (GPIOC)

int main() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    BTN_PORT->MODER &= ~(0x3 << (BTN_PIN * 2)); // Clear mode bits, set user btn to input
    
    // Check button state (assuming active high)
    if ((BTN_PORT->IDR & (1 << BTN_PIN)) == 1)
    {
        while ((BTN_PORT->IDR & (1 << BTN_PIN)) == 0)
        ; // Wait for button release
    }
    
}