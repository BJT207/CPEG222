/**********************************************************************
 * Name: ultrasonic Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/22/2026 (mm/dd/yyyy)
 * Creation Date: 10/22/2026 (mm/dd/yyyy)
 * Main Board: STM32 Nucleo F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
    * CPEG222 Robot Base Rev.C
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    *  Used to drive a HC-SR04 Ultrasonic Sensor
**********************************************************************/
#ifndef ultrasonic_driver
#define ultrasonic_driver
#include "cpeg222_driver.h"
    // Pin Definintions
    #define ultrasonic_trigger_pin 3
    #define ultrasonic_echo_pin 1
    #define ultrasonic_trigger_GPIO GPIOE
    #define ultrasonic_echo_GPIO GPIOG
    void HCSR04_Trigger(void);
    // Inline Macros
    static inline void ENABLE_ULTRASONIC(uint32_t Prescaler, uint8_t interupt_priority){
        RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // Enable timer 7
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOEEN; //Enable the pins for trigger and echo
        ultrasonic_trigger_GPIO->MODER &= ~(0x3 << (ultrasonic_trigger_pin * 2));
        ultrasonic_trigger_GPIO->MODER |= OUTPUT_MODE << (ultrasonic_trigger_pin * 2); // set trigger to output
        ultrasonic_echo_GPIO->MODER &= ~(0x3 << (ultrasonic_echo_pin * 2)); // set echo to input
        ultrasonic_echo_GPIO->MODER |= INPUT_MODE << (ultrasonic_echo_pin * 2);
        TIM7->PSC = Prescaler -1; // prescaller to set the time units of the counter
        TIM7->ARR = 0xFFFF; // 16-bit max period for free running timing
        TIM7->EGR = TIM_EGR_UG; // Generate an update event to load the prescaler value
        TIM7->CR1 |= TIM_CR1_CEN;
        NVIC_DisableIRQ(EXTI1_IRQn); // ensure echo pin interupts are disabled while booting
        NVIC_SetPriority(TIM7_IRQn, interupt_priority); /*Set priority for TIM7*/
        SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1);
        SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PG; //maps EXTI to PG1
        EXTI->IMR |= (EXTI_IMR_IM1); // sets up EXTI to trigger interrupt 1
        EXTI->RTSR |= EXTI_RTSR_TR1; // sets up rising edge trigger
        EXTI->FTSR |= EXTI_FTSR_TR1; // sets up falling edge trigger
        EXTI->PR = EXTI_PR_PR1;
        NVIC_ClearPendingIRQ(EXTI1_IRQn);
        NVIC_EnableIRQ(EXTI1_IRQn); // enables echo pin interupts
    }
    
#endif