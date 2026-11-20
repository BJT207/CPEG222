/**********************************************************************
 * Name: Line Follower Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/19/2026 (mm/dd/yyyy)
 * Creation Date: 11/19/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Used to drive the line follower IR sensor
**********************************************************************/
#ifndef LINEFOLOWERHEADER
#define LINEFOLOWERHEADER
    #include "stm32f446xx.h"
    #include "cpeg222_driver.h"

    #define IR1_ADC_CHANNEL 3
    #define IR2_ADC_CHANNEL 10
    #define IR3_ADC_CHANNEL 13
    #define IR4_ADC_CHANNEL 9
    #define IR5_ADC_CHANNEL 15

    volatile uint16_t adc3_buffer[5];

    static inline void ENABLE_IR_SENSOR() {
        RCC->AHB1ENR |= IR_GPIOEN_MKS | RCC_AHB1ENR_DMA2EN;
        RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
        // Set the mode to Analog for all the IR sensor pins
        IR1_GPIO->MODER &= ~(0x3 << IR1);
        IR2_GPIO->MODER &= ~(0x3 << IR2);
        IR3_GPIO->MODER &= ~(0x3 << IR3);
        IR4_GPIO->MODER &= ~(0x3 << IR4);
        IR5_GPIO->MODER &= ~(0x3 << IR5);
        IR1_GPIO->MODER &= ANALOG_MODE << IR1;
        IR2_GPIO->MODER &= ANALOG_MODE << IR2;
        IR3_GPIO->MODER &= ANALOG_MODE << IR3;
        IR4_GPIO->MODER &= ANALOG_MODE << IR4;
        IR5_GPIO->MODER &= ANALOG_MODE << IR5; 

        // Make sure DMA is off before tweaking it
        DMA2_Stream0->CR &= ~DMA_SxCR_EN;

        DMA2_Stream0->PAR &= 0x0000;
        DMA2_Stream0->PAR |= (uint32_t)(ADC3->DR);
        DMA2_Stream0->M0AR &= 0x0000;
        DMA2_Stream0->M0AR |= (uint32_t)adc3_buffer;
        
        // Set up DMA going from the ADC to the Memory
        DMA2_Stream0->CR = (2U << DMA_SxCR_CHSEL_Pos) |  // Channel 2 (ADC3)
                        DMA_SxCR_MSIZE_0           |  // Memory size: 16-bit (Half-word)
                        DMA_SxCR_PSIZE_0           |  // Peripheral size: 16-bit (Half-word)
                        DMA_SxCR_MINC              |  // Memory increment mode
                        DMA_SxCR_CIRC              |  // Circular mode
                        (0U << DMA_SxCR_DIR_Pos);     // Direction: Peripheral-to-memory (00)

        DMA2_Stream0->CR |= DMA_SxCR_EN; // Enable DMA2 Controller

        ADC->CCR &= ~ADC_CCR_ADCPRE;
        ADC->CCR |= ADC_CCR_ADCPRE_0;                    // PCLK2 divided by 4

        // Configure the ADC
        
        ADC3->CR1 = ADC_CR1_SCAN;                        // Enable Scan mode for multi-channel

        ADC3->CR2 = ADC_CR2_CONT |                       // Continuous conversion mode
                    ADC_CR2_DMA  |                       // Enable DMA mode
                    ADC_CR2_DDS;                         // lets DMA infinitely circle
        // Set the sample to max for all the channels used
        ADC3->SMPR2 |= (0b111 << (3 * 3)) |
                    (0b111 << (9 * 3)) ;
        ADC3->SMPR1 |= (0b111 << (0 * 3)) |
                    (0b111 << (3 * 3)) |
                    (0b111 << (5 * 3)) ;

        ADC3->SQR1 &= ~ADC_SQR1_L;
        ADC3->SQR1 |= (4U << ADC_SQR1_L_Pos); // Sets the lenght of the loop to 5

        // Set up the loop list
        ADC3->SQR3 = (3U  << ADC_SQR3_SQ1_Pos) |
                    (10U << ADC_SQR3_SQ2_Pos) |
                    (13U << ADC_SQR3_SQ3_Pos) |
                    (9U  << ADC_SQR3_SQ4_Pos) |
                    (15U << ADC_SQR3_SQ5_Pos);
        ADC3->CR2 |= ADC_CR2_ADON;
        
        // Short stabilization delay (tSTAB ~ 3us)
        for (volatile int i = 0; i < 1000; i++);

        ADC3->CR2 |= ADC_CR2_SWSTART;
    }
#endif