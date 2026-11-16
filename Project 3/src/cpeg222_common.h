/**********************************************************************
 * Name: CPEG222 Common
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 9/25/2026 (mm/dd/yyyy)
 * Creation Date: 9/24/2026 (mm/dd/yyyy)
 * Description:
    * Common pins and startup calls used for CPEG222 @ UDEL
    * Board is the STM NUCLEO F446ZE
**********************************************************************/
#include "stm32f446xx.h"

#ifndef CPEG222_COMMON
#define CPEG222_COMMON

    // constant values
        const unsigned char digitSegments[] = {
            0b1000000, // 0
            0b1111001, // 1
            0b0100100, // 2
            0b0110000, // 3
            0b0011001, // 4
            0b0010010, // 5
            0b0000010, // 6
            0b1111000, // 7
            0b0000000, // 8
            0b0010000, // 9
	        0b1111111  // blank/off
        };


    
    // General
        #define OUTPUT_MODE 0x1
        #define INPUT_MODE 0x0

    // On the Board itself
        //LEDS
        #define B_RED_LED 14
        #define B_BLUE_LED 7
        #define B_GREEN_LED 0
        #define B_LED_GPIO GPIOB

        // Board(User) Button Stuff
        #define B_BTN_PIN 13
        #define B_USR_BTN_PRESSED ((GPIOC->IDR & (1<<BTN_PIN)) != 0)

    // SHIELD
        // PINS
            // LEDS
                #define LED1_H 0
                #define LED2_H 1
                #define LED3_H 2
                #define LED4_H 3
                #define LED5_H 4
                #define LED6_H 5
                #define LED7_H 6
                #define LED8_H 7
            // Switches
                #define SW1_H 8
                #define SW2_H 9
                #define SW3_H 10
                #define SW4_H 11
            // Buttons
                #define BTN_UP_H     7
                #define BTN_DOWN_H   5
                #define BTN_LEFT_H   9
                #define BTN_RIGHT_H  6
                #define BTN_CENTER_H 8
            // Analog
                #define V_POT_H 2
        // GPIO DEFS
            #define LED_GPIO_HAT GPIOD
            #define SW_GPIO_HAT GPIOC


    // Clears and then sets the enable register inputs for the shield buttons 
        #define ENABLE_SHIELD_BUTTONS \
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN; \
            GPIOE->MODER &= ~((0x3 << (BTN_DOWN_H * 2)) | (0x3 << (BTN_RIGHT_H * 2))); \
            GPIOF->MODER &= ~((0x3 << (BTN_UP_H * 2)) | (0x3 << (BTN_LEFT_H * 2)) | (0x3 << (BTN_CENTER_H * 2))); \
            GPIOE->MODER |= (INPUT_MODE << (BTN_DOWN_H * 2)) + (INPUT_MODE << (BTN_RIGHT_H * 2)); \
            GPIOF->MODER |= (INPUT_MODE << (BTN_UP_H * 2)) | (INPUT_MODE << (BTN_CENTER_H * 2)) | (INPUT_MODE << (BTN_LEFT_H * 2));
        
    // Clears and then sets the enable register to output for the shield LEDs
        #define ENABLE_SHIELD_LEDS \
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; \
            GPIOD->MODER &= ~((0x3 << (LED1_H * 2)) | (0x3 << (LED2_H * 2)) | (0x3 << (LED3_H * 2)) | (0x3 << (LED4_H * 2)) | \
                            (0x3 << (LED5_H * 2)) | (0x3 << (LED6_H * 2)) | (0x3 << (LED7_H * 2)) | (0x3 << (LED8_H * 2))); \
            GPIOD->MODER |= (OUTPUT_MODE << (LED1_H * 2)) | (OUTPUT_MODE << (LED2_H * 2)) | (OUTPUT_MODE << (LED3_H * 2)) | \
                            (OUTPUT_MODE << (LED4_H * 2)) | (OUTPUT_MODE << (LED5_H * 2)) | (OUTPUT_MODE << (LED6_H * 2)) | \
                            (OUTPUT_MODE << (LED7_H * 2)) | (OUTPUT_MODE << (LED8_H * 2));

    // Clears and then sets up the interupts for all the buttons
        #define ENABLE_SHIELD_BUTTON_INTERUPTS \
            SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI5 | SYSCFG_EXTICR2_EXTI6 | SYSCFG_EXTICR2_EXTI7); \
            SYSCFG->EXTICR[2] &= ~(SYSCFG_EXTICR3_EXTI8 | SYSCFG_EXTICR3_EXTI9); \
            SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PE | SYSCFG_EXTICR2_EXTI6_PE | SYSCFG_EXTICR2_EXTI7_PF; \
            SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PF | SYSCFG_EXTICR3_EXTI9_PF; \
            EXTI->IMR |= (1 << BTN_UP_H) | (1 << BTN_DOWN_H) | (1 << BTN_LEFT_H) | (1 << BTN_RIGHT_H) | (1 << BTN_CENTER_H); \
            EXTI->RTSR |= (1 << BTN_UP_H) | (1 << BTN_DOWN_H) | (1 << BTN_LEFT_H) | (1 << BTN_RIGHT_H) | (1 << BTN_CENTER_H); \
            NVIC_EnableIRQ(EXTI9_5_IRQn);

    // Clears and then sets the enable register inputs for the shield switches 
        #define ENABLE_SHIELD_SWITCHES \
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; \
            GPIOC->MODER &= ~((0x3 << (SW1_H * 2)) | (0x3 << (SW2_H * 2)) | (0x3 << (SW3_H * 2)) |(0x3 << (SW4_H * 2))); \
            GPIOC->MODER |= (INPUT_MODE << (SW1_H * 2)) | (INPUT_MODE << (SW2_H * 2)) | \
                            (INPUT_MODE << (SW3_H * 2)) | (INPUT_MODE << (SW4_H * 2));

    // Enables the ADC to read from the V Potentiomiter on the hat
        #define ENABLE_V_POT_HAT_INPUT \
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; \
            GPIOC->MODER &= ~(0x3 << (V_POT_H * 2)); \
            GPIOC->MODER |= (0x3 << (V_POT_H * 2)); \
            RCC->APB2ENR |= RCC_APB2ENR_ADC3EN; /* Enable ADC3 clock*/ \
            ADC3->SQR3 = 12; /*Select channel*/ \
            ADC3->SMPR2 = ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP1_1; /*Sample time 56 cycles*/\
            ADC3->CR2 = ADC_CR2_ADON /*Enable ADC*/;
    
    // Enables the SYSTICK timer and sets the counter increments        
        #define ENABLE_SYS_TICK(COUNTER) \
            SysTick->LOAD = COUNTER-1; /*Load value to count up to*/ \
            SysTick->VAL = 0; /* Clear current value */ \
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk  | \
                            SysTick_CTRL_TICKINT_Msk    | \
                            SysTick_CTRL_ENABLE_Msk; \
            NVIC_SetPriority(SysTick_IRQn, 1); 	/* Set the SysTick priority */

        #define ENABLE_7SD_HAT \


    
    void display_digit(uint8_t digit, uint8_t number) {
        return;
    }

    
#endif