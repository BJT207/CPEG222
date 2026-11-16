/********************************************************************
 * Project 2
 * Date: 9/24/26 (mm/dd/yyyy)
 * Class: CPEG222
 * Author: Brandon Trainor
 * Board: NucleoF466ZE 
 * CMSIS Header: STM32F4xx
 * Description
    * Demo of interupts using the buttons, switches, and leds on the
    * CPEG222 Trainer Brd
*********************************************************************/

// Imports
#include <stdbool.h>
#include "stm32f4xx.h"
#include "unit_values.h" //Personal Header File with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
#include "cpeg222_common.h" // Personal Header with common pin names for the NANO F446ZE board

// Variables
uint8_t led_position = 0; // position value for the led
uint8_t state = 0; // 0 is paused, 1 is right, any other value is left

bool leds[4] = {false,false, false, false}; // the leds that are currently on

volatile uint32_t ms_ticks = 0; // SysTick counter

volatile uint32_t last_interrupt_time[3] = {0,0,0}; // the last time in ms that each button was pressed {center, left, right}

uint32_t last_shift_time = 0; // last time the leds shifted (ms_ticks)
uint32_t shift_delay = 0; // time in ms of delay shifting 

// Lookup Tables
const uint8_t precal_modulo[15] = { // used to save cycles calculating (position + movement shift) % 8
    0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6
};

// Interupts
void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        uint32_t current_time = SysTick->VAL;
        if (current_time - last_interrupt_time[0] > 50) { // 50 clock cycles debounce threshold
            state = 0;
            last_interrupt_time[0] = current_time;
        }
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_LEFT_H)) {
        uint32_t current_time = SysTick->VAL;
        if (current_time - last_interrupt_time[1] > 50) { // 50 clock cycles debounce threshold
            state = 2;
            last_interrupt_time[1] = current_time;
        }
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        uint32_t current_time = SysTick->VAL;
        if (current_time - last_interrupt_time[2] > 50) { // 50 clock cycles debounce threshold
            state = 1;
            last_interrupt_time[2] = current_time;
        }
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_UP_H)) {
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_DOWN_H)) {
        EXTI->PR |= (1 << BTN_DOWN_H); // Clear Interupt bit
    }
    
}

void SysTick_Handler(void) {
    ms_ticks++;
}

// Helper Functions

void shift(bool direction, uint32_t shift_delay) { // Used to shift the led position values
    if ((ms_ticks - last_shift_time) >= shift_delay) {
        led_position = (precal_modulo[led_position + (direction ? 7 : 1)]);
        last_shift_time = ms_ticks;
    }
}

// Functions
int main(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock

    ENABLE_SHIELD_LEDS //Enables the LED pins to OUTPUT on the shield (GPIO D)
    ENABLE_SHIELD_SWITCHES // Clears and then sets the enable register inputs for the shield switches 
    ENABLE_SHIELD_BUTTONS //Enables the buttons on the shield (GPIO E and F)
    ENABLE_V_POT_HAT // Sets up the V_POT as well as the ADC at a sample time of 56 cycles
    ENABLE_SHIELD_BUTTON_INTERUPTS // Clears and then sets up the interupts for all the buttons

    SysTick_Config(SystemCoreClock/1000);
    while (1) {
        if (state == 0) {
            led_position = 0;
            leds[0] = (bool)((SW_GPIO_HAT->IDR & (1 << SW1_H))  != 0);
            leds[1] = (bool)((SW_GPIO_HAT->IDR & (1 << SW2_H))  != 0);
            leds[2] = (bool)((SW_GPIO_HAT->IDR & (1 << SW3_H))  != 0);
            leds[3] = (bool)((SW_GPIO_HAT->IDR & (1 << SW4_H))  != 0);
        }
        else {
            uint32_t total = 0;
            for(int i = 0; i < 16; i++){
                ADC3->CR2 |= ADC_CR2_SWSTART; // Start conversion
                while(!(ADC3->SR & ADC_SR_EOC)); // Wait for conversion to complete
                total += ADC3->DR; // Read data
            }
            shift_delay = (uint32_t)(total / 16); // Convert to steped value

            if (state == 1) {
                shift(true, shift_delay);
            }
            else {
                shift(false, shift_delay);
            }
        }
        LED_GPIO_HAT->BSRR = (0xffff << 16)                                 | 
                             (leds[0] << led_position)                      | 
                             (leds[1] << (precal_modulo[led_position + 1])) | 
                             (leds[2] << (precal_modulo[led_position + 2])) | 
                             (leds[3] << (precal_modulo[led_position + 3]));

    }

    return 0;
}