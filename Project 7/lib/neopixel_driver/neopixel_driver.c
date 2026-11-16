/************************************************************************
 * Name: Neopixel Driver
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
#include "neopixel_driver.h"
#include <stdio.h>

void NeoPixel_SetColor(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index >= NUM_LEDS) return;
        // WS2812B expects GRB order MSB first
        uint32_t grb = ((uint32_t)green << 16) | ((uint32_t)red << 8) | blue;
        uint32_t start_idx = index * BITS_PER_LED;
        for (int i = 0; i < 24; i++) {
            // Extract MSB to LSB
            if (grb & (1U << (23 - i))) {
                dma_pwm_buffer[start_idx + i] = PWM_1_VAL;
            } else {
                dma_pwm_buffer[start_idx + i] = PWM_0_VAL;
            }
        }
}

void NeoPixel_Show() {
    // Stop DMA and Timer Neopixel Show
    TIM2->CR1 &= ~TIM_CR1_CEN;
    DMA1_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA1_Stream7->CR & DMA_SxCR_EN);
    // Force CCR4 to 0 (output low) without raising a DMA request
    TIM2->DIER &= ~TIM_DIER_UDE;
    TIM2->CCR4 = 0;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0;
    // Reset DMA transfer counter and clear all stream 7 flags
    DMA1_Stream7->M0AR = (uint32_t)dma_pwm_buffer;
    DMA1_Stream7->NDTR = BUFFER_LEN;
    DMA1->HIFCR = DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 |
    DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7;
    // Each update event (one per bit period) makes DMA write the next compare value
    DMA1_Stream7->CR |= DMA_SxCR_EN;
    TIM2->CNT = 0;
    TIM2->DIER |= TIM_DIER_UDE;
    TIM2->CR1 |= TIM_CR1_CEN;
}