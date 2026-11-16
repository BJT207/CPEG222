// ****************************************************************
// * Authors: T. LUM and R. MARTIN
// * Modified by B. Trainor
// * CPEG222 Demo Program5, 9/28/2026
// * NucleoF446ZE CMSIS STM32F4xx example Proj5
// * Sends text to the OLED display using SPI
// ****************************************************************
#include "stm32f4xx.h"
#include "OLED.h"
#include "SysClock180MHz.h"
#include "cpeg222_common.h"
#include "unit_values.h"
#include <stdint.h> // Include stdint.h for uint32_t type
#include <stdio.h> // Include stdio.h for snprintf function
#include <stdbool.h>

#define SECOND (int)(180*MEGA)

void delay_ms(uint32_t ms); // Function prototype for delay function
char buffer[90];
uint32_t initialClock; // Initial system clock frequency (16 MHz)
uint32_t finalClock;   // Final system clock frequency (180 MHz)
volatile uint32_t ms_ticks[5] = {0, 0, 0, 0, 0};
volatile uint32_t position[2] = {3,0};
volatile bool update_flag = false;
volatile bool invert_flag = false;

void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        if (ms_ticks[0] >= 50) { // 50 ms debounce
            ms_ticks[0] = 0U;
        }
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_LEFT_H)) {
        if (ms_ticks[3] >= 50) { // 50 ms debounce
            ms_ticks[3] = 0U;
            if (position[1] > 0U) {
                position[1] -= 6U;
            }
            update_flag = true;
        }
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        if (ms_ticks[4] >= 50) { // 50 ms debounce
            ms_ticks[4] = 0U;
            if (position[1] < 36U) {
                position[1] += 6U;
            }
            update_flag = true;
        }
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_UP_H)) {
        if (ms_ticks[1] >= 50) { // 50 ms debounce
            ms_ticks[1] = 0U;
            if (position[0] > 2U) {
                position[0] -= 1U;
            }
            update_flag = true;
        }
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_DOWN_H)) {
        if (ms_ticks[2] >= 50) { // 50 ms debounce
            ms_ticks[2] = 0U;
            if (position[0] < 6U) {
                position[0] += 1U;
            }
            update_flag = true;
        }
        EXTI->PR |= (1 << BTN_DOWN_H); // Clear Interupt bit
    }   
}

void SysTick_Handler(void) {
    for (int i = 0; i < 5; i++) { // debounce delay for buttons
        if (ms_ticks[i] < 50) {
            ms_ticks[i] += 1U;
        }
    }
}

int main(void){
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
    SystemCoreClockUpdate(); // CMSIS function to update SystemCoreClock variable
    initialClock = SystemCoreClock; // Store the initial clock frequency (16 MHz)
    SystemClock_Config(); // Configure the system clock to 180 MHz
    SystemCoreClockUpdate(); // Update SystemCoreClock variable after clock change
    finalClock = SystemCoreClock; // Store the final clock frequency (180 MHz)
    OLED_Init();
    SH1106_RenderFullScreenLogo();
    delay_ms(4000); // Delay for 4 seconds to display the logo
    OLED_Clear();
    OLED_Print(0, 0, "CPEG222-F26 OLED Test");
    snprintf(buffer, sizeof(buffer), "Initial Clock: %lu MHz  ", initialClock / 1000000U);               
    OLED_Print(5, 0, buffer);
    snprintf(buffer, sizeof(buffer), "Final Clock: %lu MHz  ", finalClock / 1000000U);
    OLED_Print(6, 0, buffer);
    delay_ms(8000); // Delay for 8 seconds to display the clock information
    SysTick_Config((int)(SECOND*MILLI)-1);
    ENABLE_SHIELD_BUTTONS();
    ENABLE_SHIELD_BUTTON_INTERUPTS(1);
    ENABLE_SHIELD_SWITCHES();
    OLED_Clear();
    OLED_Print(0, 0, "CPEG222-F26 OLED Test");
    OLED_Print(1, 0, "10/15/2026");
    OLED_Print(3, 0, "Brandon Trainor");

    

    while(1){
        if (update_flag) {
            OLED_Clear();
            OLED_Print(0, 0, "CPEG222-F26 OLED Test");
            OLED_Print(1, 0, "10/15/2026");
            OLED_Print(position[0], position[1], "Brandon Trainor");
            update_flag = false;
        }
        if(read_switch(SW1_H) != invert_flag) {
            invert_flag = !(invert_flag);

            switch((int)(invert_flag)) {
                case true:
                    OLED_WriteCmd(0b10100111);
                    break;
                case false:
                    OLED_WriteCmd(0b10100110);
                    break;
                default:
                    break;
            }

        }

    }   
    return 0; // Program should never reach here due to infinite loop
}
void delay_ms(uint32_t ms){
    if(SystemCoreClock == 16000000U){ // If the system clock is 16 MHz
        for(uint32_t i = 0; i < ms * 2000; i++){
            __NOP();
        }
    } else if(SystemCoreClock == 180000000U){ // If the system clock is 180 MHz
        for(uint32_t i = 0; i < ms * 22500; i++){
            __NOP();
        }
    }
}