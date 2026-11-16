/**********************************************************************
 * Name: Speaker Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/05/2026 (mm/dd/yyyy)
 * Date Created:  10/07/2026 (mm/dd/yyyy)
 * Description:
    * Music notes and their corresponding frequencies in Hz for use
    * with a DAC output 
**********************************************************************/
#ifndef speaker_driver
#define speaker_driver
#include <stdint.h>
#include "cpeg222_driver.h"

// Static Vars
    const uint16_t sine_lut[100];
// "Functions"
    #define speaker_set_frequency(frequency) TIM6->ARR = (uint32_t)((1000000/(frequency * 100)) - 1);
    #define speaker_set_amplitude(amplitude) DAC->DHR12R1 = amplitude;
// Settup Functions
    static inline void ENABLE_DAC_SPEAKER(uint32_t BootFrequency) {
            /**************************************************
             * Enable Digital to Analog Converter Output 
             * description:
             *      Used to enable the pins for the DAC to drive
             *      the Trainer Brd hat. Also 
             *      configures a timer to update the DAC via
             *      DMA
             * Inputs:
             *      PRESCALLER (uint16_t): the prescaller value
             *          used to configure the refresh timer
             *      PERIOD (uint32_t): the period used to 
             *          configure the timer 5 interupt for 
             *          screen refreshes
            *************************************************/ 
            /*Enable DAC*/ 
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  /* Enable GPIOA */            
                GPIOA->MODER |= (ANALOG_MODE << (DAC_H * 2)); /* Enables DAC function */  
            /*Enables the conection between DMA and the DAC via channel 7*/ 
                RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; /*Enable DMA clock*/ 
                DMA1_Stream5->CR &= ~DMA_SxCR_EN; /*temp disable DMA stream configuring*/ 
                while (DMA1_Stream5->CR & DMA_SxCR_EN); /*Wait until the stream is disabled*/ 
                DMA1_Stream5->CR |= (7U << DMA_SxCR_CHSEL_Pos); /*Set DMA to rount over Stream 5 (REQUIRED FOR DAC1 by the manual)*/ 
                DMA1_Stream5->CR |= (0x1 << DMA_SxCR_MSIZE_Pos) | /*set the Data Sizes sent between the DAC and the Memory to 16bit*/ 
                                    (0x1 << DMA_SxCR_PSIZE_Pos); 
                DMA1_Stream5->CR |= DMA_SxCR_MINC; /*sets the pointer tick up the table*/ 
                DMA1_Stream5->CR |= (0b01 << DMA_SxCR_DIR_Pos); /* set the direction of the DMA: Memory-->DAC */ 
                DMA1_Stream5->CR |= DMA_SxCR_CIRC; /*Enable Circular Mode to wrap arround the table at the end*/ 
                DMA1_Stream5->NDTR = 100; /*set the size of the lookup table*/ 
                DMA1_Stream5->PAR = (uint32_t)&(DAC->DHR12R1); /* set DAC1 to output data in a 12 bit Right-Aligned format*/ 
                DMA1_Stream5->M0AR = (uint32_t)sine_lut; /*set the pointer to the lookup table*/ 
                DMA1_Stream5->CR |= DMA_SxCR_EN; /*re-enable the DMA stream*/ 
            /*Enable the DAC*/ 
                RCC->APB1ENR |= RCC_APB1ENR_DACEN; 
            /*Configure the DAC to look for timer 6 as a trigger to go to the next LUT value*/ 
                DAC->CR &= ~DAC_CR_TSEL1; /*Select TIM6 TRGO as LUT shift Trigger */ 
                DAC->CR |= DAC_CR_TEN1; /*Enable Triggering for Channel 1*/ 
                DAC->CR |= DAC_CR_DMAEN1; /*Enable DMA Request Generation*/ 
                DAC->CR |= DAC_CR_EN1; /*Enable DAC Channel 1*/ 
            /*Config timer 6 as the DAC trigger source*/ 
                RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; 
                TIM6->CR1 &= ~TIM_CR1_CEN; /*Ensure TIM6 is disabled before configuring it*/
                TIM6->PSC = (uint32_t)(44); /*prescales to 1kHz*/ 
                TIM6->ARR = (uint32_t)((1000000/(BootFrequency*100)) - 1U); /*Auto-reload when Count = period*/ 
                /*Select Update Event as Trigger Output*/ 
                TIM6->CR2 &= ~TIM_CR2_MMS; 
                TIM6->CR2 |= (0x2 << TIM_CR2_MMS_Pos);
                TIM6->CR1 |= TIM_CR1_CEN; /*Enable timer 6*/ 
            /*Enable the the speaker*/ 
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  /* Enable the speaker AMP */  
                GPIOB->MODER &= ~(3U << (AUDIO_SD_H * 2)); /*Clear PB6*/ 
                GPIOB->MODER |= (1U << (AUDIO_SD_H * 2)); /*Set PB6 to OUTPUT Mode*/ 
                // GPIOB->BSRR = (1U << AUDIO_SD_H); /*Set PB6 high to enable audio amplifier*/ 
        }
// Inline Functions
    inline void enable_speaker(){
        /**************************************************
         * Enable Speaker 
         * description:
         *      Used to enable output on the speaker
        *************************************************/ 
        GPIOB->BSRR = (1U << AUDIO_SD_H);
    }
    inline void disable_speaker(){
        /**************************************************
         * Enable Speaker 
         * description:
         *      Used to enable output on the speaker
        *************************************************/ 
        GPIOB->BSRR = (1U << (AUDIO_SD_H+16));
    }

#endif