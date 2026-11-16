/**********************************************************************
 * Name: Project 4
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/07/2026 (mm/dd/yyyy)
 * Creation Date: 10/06/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Implements a DAC synthesizer and happy birthday player using the 
    * Nucleo F446ZE and the CPEG222 Trainer Brd Rev.E
**********************************************************************/
// Header Files
#include <stdbool.h>
#include <stm32f4xx.h>

// Personal Header Files
#include "unit_values.h" //Personal header file with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
#include "cpeg222_common.h" // Personal header with common pin names for the NANO F446ZE board
#include "music_notes.h" //Personal header file with defs for integer approximated notes
// Defines
#define SECOND (int)(180*MEGA)
#define APB1SECOND (int)(45*MEGA)
#define APB2SECOND (int)(90*MEGA)
#define DELAY 2
//Notes


// Vars
    volatile static uint8_t index = 0;
    volatile static uint8_t digit_selector = 0;
    volatile static uint32_t total = 0;
    volatile static bool happy_birthday = false;
    volatile static uint32_t counter = 0;
    volatile static uint32_t systick_prescaler = 1;
    volatile static uint32_t prescaler_counter = 0;
// Constants
    static const int place[] = {1,10,100,1000};
    static const uint32_t music[] = 
    {G3,0,G3,0,A3,A3,A3,0,G3,G3,G3,0,C4,C4,C4,0,B3,B3,B3,B3,B3,B3,B3,0,
     G3,0,G3,0,A3,A3,A3,0,G3,G3,G3,0,D4,D4,D4,0,C4,C4,C4,C4,C4,C4,C4,0,
     G3,0,G3,0,G4,G4,G4,0,E4,E4,E4,0,C4,C4,C4,0,B3,B3,B3,0,A3,A3,A3,A3,A3,A3,0,
     F4,0,F4,0,E4,E4,E4,0,C4,C4,C4,0,D4,D4,D4,0,C4,C4,C4,C4,C4,C4,C4,0,0};

// Handlers
void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_LEFT_H)) {
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_UP_H)) {
        happy_birthday = true;
        counter = 0;
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_DOWN_H)) {
        EXTI->PR |= (1 << BTN_DOWN_H); // Clear Interupt bit
    }   
}

void TIM5_IRQHandler(void){
    if(TIM5->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
        // Display the value on the SSD array
        int display_num = (total/place[digit_selector]) % 10;
        if (display_num == 0 && digit_selector == 3) {
            display_num = 10;
        }
        ssd_display_digit(digit_selector, display_num, false);
        digit_selector = (digit_selector + 1) % 4; // Cycle through digitSelect values 0 to 3
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }
}

// void TIM6_DAC_IRQHandler(void) {
//     if(TIM6->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
//         TIM6->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
//         speaker_set_amplitude(sine_lut[index]);
//         index = (index +1) % 100;
//     }
// }

void SysTick_Handler(void) {
    prescaler_counter += 1u;
    if (prescaler_counter >= systick_prescaler) {
        prescaler_counter = 0;
        if (happy_birthday){
            if (counter == UINT32_MAX) {
                counter = 0;
            }
            else{counter++;}
        }
    }
}

int main() {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
    ENABLE180MHZCLOCK();
    // Settup Macros
        ENABLE_SHIELD_BUTTONS(); //Enables the buttons on the shield (GPIO E and F)
        ENABLE_SHIELD_BUTTON_INTERUPTS(1); // Enables the button interupts
        ENABLE_POT_INPUT();
        ENABLE_SHIELD_SWITCHES();
        ENABLE_7SD((int)(TEN_E_N4*APB1SECOND), 25,1); // Sets up timer 5 and sets up the pins for the 7 seg display
        ENABLE_SYS_TICK((int)(SECOND*MILLI),(int)(175), 1); // Sets up SysTick and SysTick interupt
        ENABLE_DAC_SPEAKER(sine_lut, 100U,(uint16_t)(APB1SECOND/(440 * 100)));
    // 
    
    while(1) {
        if (happy_birthday) {
            if (counter >= (101+DELAY)) {
                happy_birthday = false;
                GPIOB->BSRR = (1U << (AUDIO_SD_H+16));
            }
            else if (counter >= DELAY) {
                if (music[counter-DELAY]) {
                    speaker_set_frequency((APB1SECOND/(music[counter-DELAY]*100)));
                    GPIOB->BSRR = (1U << AUDIO_SD_H);
                    total = music[counter-DELAY];                    
                }
                else {
                    GPIOB->BSRR = (1U << (AUDIO_SD_H+16));
                    total = 0;
                }
            }
        }
        else {
            uint32_t temp = 0U;
            read_Hat_POT(&temp);
            total = (uint16_t)((((temp)*1900)/4096) + 100);
            speaker_set_frequency((APB1SECOND/(total*100)));
            if (read_switch(SW1_H)) {
                GPIOB->BSRR = (1U << (AUDIO_SD_H+16));
            }
            else {
                GPIOB->BSRR = (1U << AUDIO_SD_H);
            }
        }
        
    };
}