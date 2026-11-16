/************************************************************************
 * Name: Neopixel Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/29/2026 (mm/dd/yyyy)
 * Creation Date: 10/28/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
 * Description:
    * Driving Functions for the Neopixels on the CPEG222 Trainor Red Rev.E
 * Attribution:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
************************************************************************/
#ifndef NEOPIXEL_DRIVR
#define NEOPIXEL_DRIVR
    #include <stdio.h>
    #include "stm32f446xx.h"

    #define NUM_LEDS 4
    #define BITS_PER_LED 24
    #define RESET_SLOTS 50
    #define PWM_0_VAL 36 // ~0.40 us High Pulse at 90 MHz TIM2 clock
    #define PWM_1_VAL 72 // ~0.80 us High Pulse at 90 MHz TIM2 clock
    #define BUFFER_LEN ((NUM_LEDS * BITS_PER_LED) + RESET_SLOTS)
    uint32_t dma_pwm_buffer[BUFFER_LEN];
    
    void NeoPixel_Show();
    void NeoPixel_SetColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
    void NeoPixel_Display_Solid_Color(uint8_t red, uint8_t green, uint8_t blue);
    
    static inline void ENABLE_NEOPIXELS(){
        // 1. Enable Clocks for GPIOB, TIM2, and DMA1
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_DMA1EN;
            RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
            // An off pixel is still transmitted as 24 zero bits, not as a zero compare value.
            for (uint32_t i = 0; i < NUM_LEDS * BITS_PER_LED; i++) {
            dma_pwm_buffer[i] = PWM_0_VAL;
            }
        // 2. Configure PB11 as TIM2_CH4 Alternate Function (AF1)
            GPIOB->MODER &= ~(3U << (11 * 2));
            GPIOB->MODER |= (2U << (11 * 2)); // AF Mode (10)
            GPIOB->OSPEEDR |= (3U << (11 * 2)); // High speed for clean WS2812 edges
            // PB11 is in AFR High Register (AFR[1]) at position (11 - 8) * 4 = 12
            GPIOB->AFR[1] &= ~(0xFU << ((11 - 8) * 4));
            GPIOB->AFR[1] |= (0x1U << ((11 - 8) * 4)); // AF1 for TIM2
        // 3. Configure TIM2 Channel 4 in PWM Mode 1
            TIM2->PSC = 0; // Full 90 MHz APB1 timer clock
            TIM2->ARR = 111; // 800 kHz PWM frequency (1.25 us period)
            TIM2->CCR4 = 0;
            // Configure Channel 4 PWM Mode 1 & enable output
            TIM2->CCMR2 &= ~TIM_CCMR2_OC4M;
            TIM2->CCMR2 |= (6U << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE; // PWM Mode 1 + Preload enable
            TIM2->CCER |= TIM_CCER_CC4E; // Enable Channel 4 Output
            TIM2->DIER &= ~TIM_DIER_CC4DE; // DMA is paced by the update event (enabled in Show)
        // 4. Configure DMA1 Stream 7 Channel 3 (TIM2_UP Request)
            DMA1_Stream7->CR &= ~DMA_SxCR_EN; // Disable before configuration
            while (DMA1_Stream7->CR & DMA_SxCR_EN);
            DMA1_Stream7->PAR = (uint32_t)&(TIM2->CCR4); // Target TIM2 Compare Register 4
            DMA1_Stream7->M0AR = (uint32_t)dma_pwm_buffer;
            DMA1_Stream7->NDTR = BUFFER_LEN;
            // Channel 3, Memory-to-Peripheral, 32-bit data size, Memory increment, Normal mode
            DMA1_Stream7->CR = (3U << DMA_SxCR_CHSEL_Pos) |
            DMA_SxCR_DIR_0 |
            DMA_SxCR_MINC |
            DMA_SxCR_MSIZE_1 |
            DMA_SxCR_PSIZE_1;
    }
#endif