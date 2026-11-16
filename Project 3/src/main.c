/**********************************************************************
 * Name: Project 3
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/2/2026 (mm/dd/yyyy)
 * Creation Date: 10/1/2026 (mm/dd/yyyy)
 * Description:
    * Implements a 100ms timer using the Nucleo F446ZE and the
    * UDEL CPEG222 hat
**********************************************************************/

#include <stdbool.h>
#include "stm32f4xx.h"
#include "unit_values.h" //Personal Header File with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
#include "cpeg222_common.h" // Personal Header with common pin names for the NANO F446ZE board

#define SECOND 16*MEGA

// Vars
volatile uint8_t digit_selector = 0U;
volatile uint16_t clock_count = 0U;
volatile bool paused = true;
volatile bool up_or_down = true;
volatile uint32_t ms_ticks[5] = {0, 0, 0, 0, 0};

// Constants
const int place[] = {1,10,100,1000};

// Handlers
void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        if (ms_ticks[0] >= 5) { // 50 ms debounce
            paused = !(paused);
            ms_ticks[0] = 0U;
        }
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_LEFT_H)) {
        if (ms_ticks[3] >= 5) { // 50 ms debounce
            paused = true;
            clock_count = 0U;
            ms_ticks[3] = 0U;
        }
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        if (ms_ticks[4] >= 5) { // 50 ms debounce
            paused = true;
            clock_count = 9999U;
            ms_ticks[4] = 0U;
        }
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_UP_H)) {
        if (ms_ticks[1] >= 5) { // 50 ms debounce
            up_or_down = true;
            paused = false;
            ms_ticks[1] = 0U;
        }
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_DOWN_H)) {
        if (ms_ticks[2] >= 5) { // 50 ms debounce
            up_or_down = false;
            paused = false;
            ms_ticks[2] = 0U;
        }
        EXTI->PR |= (1 << BTN_DOWN_H); // Clear Interupt bit
    }   
}

void TIM5_IRQHandler(void){
    if(TIM5->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
        // Display the value on the SSD array
        int display_num = (clock_count/place[digit_selector]) % 10;
        if (display_num == 0 && digit_selector == 3) {
            display_num = 10;
        }
        display_digit(digit_selector, display_num);
        digit_selector = (digit_selector + 1) % 4; // Cycle through digitSelect values 0 to 3
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }
}

void SysTick_Handler(void) {
    for (int i = 0; i < 5; i++) { // debounce delay for buttons
        if (ms_ticks[i] < 5) {
            ms_ticks[i] += 1U;
        }
    }
    if (!paused && up_or_down) { // checks for up/paused
        if (clock_count < 9999) {
            clock_count += 1U;
        }
        else {
            paused = true;
        }
    }
    else if (!paused) { // checks for down/paused
        if (clock_count > 0) {
            clock_count -= 1U;
        }
        else {
           paused = true;
        }
    }
}

int main() {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
    ENABLE_SHIELD_BUTTONS; //Enables the buttons on the shield (GPIO E and F)
    ENABLE_SHIELD_BUTTON_INTERUPTS; // Enables the button interupts
    ENABLE_7SD_HAT((int)(TEN_E_N4*SECOND), 25); // Sets up timer 5 and sets up the pins for the 7 seg display
    SysTick_Config((int)(SECOND*CENTI)); // Sets up systick for 

    while(1);
}