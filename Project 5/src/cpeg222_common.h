/************************************************************************
 * Name: CPEG222 Common
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/15/2026 (mm/dd/yyyy)
 * Creation Date: 9/24/2026 (mm/dd/yyyy)
 * Main Board: STM NUCLEO F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
 * Description:
    * Pins, startup macros, and common functions used for CPEG222 @ UDEL
 * Atribition:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
************************************************************************/
#include "stm32f446xx.h"
#include <stdbool.h>
#ifndef CPEG222_COMMON
#define CPEG222_COMMON

    // Constant Values
        const unsigned char ssddigitSegments[] = {
            0b0111111, // 0
            0b0000110, // 1
            0b1011011, // 2
            0b1001111, // 3
            0b1100110, // 4
            0b1101101, // 5
            0b1111101, // 6
            0b0000111, // 7
            0b1111111, // 8
            0b1101111, // 9
            0b0000000  // blank/off
        };
        const uint16_t sine_lut[100] = {
            2048, 2110, 2173, 2235, 2296, 2357, 2416, 2474, 2530, 2584,
            2636, 2685, 2732, 2777, 2818, 2857, 2892, 2924, 2953, 2978,
            2999, 3016, 3030, 3040, 3046, 3048, 3046, 3040, 3030, 3016,
            2999, 2978, 2953, 2924, 2892, 2857, 2818, 2777, 2732, 2685,
            2636, 2584, 2530, 2474, 2416, 2357, 2296, 2235, 2173, 2110,
            2048, 1985, 1922, 1860, 1799, 1738, 1679, 1621, 1565, 1511,
            1459, 1410, 1363, 1318, 1277, 1238, 1203, 1171, 1142, 1117,
            1096, 1079, 1065, 1055, 1049, 1048, 1049, 1055, 1065, 1079,
            1096, 1117, 1142, 1171, 1203, 1238, 1277, 1318, 1363, 1410,
            1459, 1511, 1565, 1621, 1679, 1738, 1799, 1860, 1922, 1985
        };
    // General
        #define INPUT_MODE      0b00
        #define OUTPUT_MODE     0b01
        #define ALT_FUNC_MODE   0b10
        #define ANALOG_MODE     0b11

    // Board Pins
        //LEDS
        #define B_RED_LED 14
        #define B_BLUE_LED 7
        #define B_GREEN_LED 0
        #define B_LED_GPIO GPIOB

        // Board(User) Button Stuff
        #define B_BTN_PIN 13
        #define B_USR_BTN_PRESSED ((GPIOC->IDR & (1<<BTN_PIN)) != 0)

    // CPEG222 Sheild Pins
        // LEDS (GPIOD)
            #define LED1_H 0
            #define LED2_H 1
            #define LED3_H 2
            #define LED4_H 3
            #define LED5_H 4
            #define LED6_H 5
            #define LED7_H 6
            #define LED8_H 7
        // 7 Segment Display
            #define SSD_DIGIT1_SEL 10 // GPIOE
            #define SSD_DIGIT2_SEL 7  // GPIOE
            #define SSD_DIGIT3_SEL 5  // GPIOB
            #define SSD_DIGIT4_SEL 3  // GPIOB
            #define SSD_A 9  // GPIOG
            #define SSD_B 12 // GPIOF
            #define SSD_C 13 // GPIOF
            #define SSD_D 14 // GPIOF
            #define SSD_E 8  // GPIOE
            #define SSD_F 15 // GPIOF
            #define SSD_G 4  // GPIOB
            #define SSD_DEC_PNT 14 // GPIOG
        // Switches (GPIOC)
            #define SW1_H 8
            #define SW2_H 9
            #define SW3_H 10
            #define SW4_H 11
        // Buttons (GPIOD)
            #define BTN_UP_H     7
            #define BTN_DOWN_H   5
            #define BTN_LEFT_H   9
            #define BTN_RIGHT_H  6
            #define BTN_CENTER_H 8
        // Analog Devices
            #define A_POT_H  2 // GPIOC
            #define DAC_H    4 // GPIOA
            #define AUDIO_SD_H 6 // GPIOB    
    // Settup Macros 
        #define ENABLE_SHIELD_BUTTONS() \
            /******************************************
             * Enable Shield Buttons
             * Description:
             *      Used to set the enable registers 
             *      to input for all 5 of the buttons
             *      on the Trainer Brd Hat
            ******************************************/ \
            do {\
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN; \
            GPIOE->MODER &= ~((0x3 << (BTN_DOWN_H * 2)) | (0x3 << (BTN_RIGHT_H * 2))); \
            GPIOF->MODER &= ~((0x3 << (BTN_UP_H * 2)) | (0x3 << (BTN_LEFT_H * 2)) | (0x3 << (BTN_CENTER_H * 2))); \
            GPIOE->MODER |= (INPUT_MODE << (BTN_DOWN_H * 2)) + (INPUT_MODE << (BTN_RIGHT_H * 2)); \
            GPIOF->MODER |= (INPUT_MODE << (BTN_UP_H * 2)) | (INPUT_MODE << (BTN_CENTER_H * 2)) | (INPUT_MODE << (BTN_LEFT_H * 2)); \
            } while(0)
            
        
        #define ENABLE_SHIELD_LEDS() \
            /******************************************
             * Enable Shield LEDs
             * Description:
             *      Used to set the enable registers 
             *      to input for all 8 of the red leds
             *      on the Trainer Brd Hat
            ******************************************/ \
            do {\
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; \
            GPIOD->MODER &= ~((0x3 << (LED1_H * 2)) | (0x3 << (LED2_H * 2)) | \
                                (0x3 << (LED3_H * 2)) | (0x3 << (LED4_H * 2)) | \
                                (0x3 << (LED5_H * 2)) | (0x3 << (LED6_H * 2)) | \
                                (0x3 << (LED7_H * 2)) | (0x3 << (LED8_H * 2))); \
            GPIOD->MODER |= (OUTPUT_MODE << (LED1_H * 2)) | (OUTPUT_MODE << (LED2_H * 2)) | \
                            (OUTPUT_MODE << (LED3_H * 2)) | (OUTPUT_MODE << (LED4_H * 2)) | \
                            (OUTPUT_MODE << (LED5_H * 2)) | (OUTPUT_MODE << (LED6_H * 2)) | \
                            (OUTPUT_MODE << (LED7_H * 2)) | (OUTPUT_MODE << (LED8_H * 2));  \
            } while(0)

        #define ENABLE_SHIELD_BUTTON_INTERUPTS(interupt_priority) \
            /****************************************************
             * Enable Shield Button Interupts
             * Description:
             *      Used to enable hardware interupts for all 5 
             *      of the buttons on the Trainer Brd Hat. 
             *      Interupt handler must be named 
             *      EXTI9_5_IRQHandler and at a minimum detect 
             *      and clear the flags for all five buttons
            ****************************************************/ \
                do {\
                SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI5 | SYSCFG_EXTICR2_EXTI6 | SYSCFG_EXTICR2_EXTI7); \
                SYSCFG->EXTICR[2] &= ~(SYSCFG_EXTICR3_EXTI8 | SYSCFG_EXTICR3_EXTI9); \
                SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PE | SYSCFG_EXTICR2_EXTI6_PE | SYSCFG_EXTICR2_EXTI7_PF; \
                SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PF | SYSCFG_EXTICR3_EXTI9_PF; \
                EXTI->IMR |= (1 << BTN_UP_H) | (1 << BTN_DOWN_H) | (1 << BTN_LEFT_H) | (1 << BTN_RIGHT_H) | (1 << BTN_CENTER_H); \
                EXTI->RTSR |= (1 << BTN_UP_H) | (1 << BTN_DOWN_H) | (1 << BTN_LEFT_H) | (1 << BTN_RIGHT_H) | (1 << BTN_CENTER_H); \
                NVIC_SetPriority(EXTI9_5_IRQn, interupt_priority); \
                NVIC_EnableIRQ(EXTI9_5_IRQn); \
                } while(0)

        #define ENABLE_SHIELD_SWITCHES() \
            /******************************************
             * Enable Shield Switches
             * Description:
             *      Used to set the enable registers 
             *      to input for all 4 of the switches
             *      on the Trainer Brd Hat
            ******************************************/ \
            do {\
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; \
            GPIOC->MODER &= ~((0x3 << (SW1_H * 2)) | (0x3 << (SW2_H * 2)) | (0x3 << (SW3_H * 2)) |(0x3 << (SW4_H * 2))); \
            GPIOC->MODER |= (INPUT_MODE << (SW1_H * 2)) | (INPUT_MODE << (SW2_H * 2)) | \
                            (INPUT_MODE << (SW3_H * 2)) | (INPUT_MODE << (SW4_H * 2));\
            } while(0)

        #define ENABLE_POT_INPUT() \
            /******************************************
             * Enable Potentiomiter Input
             * Description:
             *      Used to set the enable registers 
             *      to analog input for the Volume
             *      potentiomiter and configure the 
             *      Analog to Digital Converter
            ******************************************/ \
            do {\
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; \
            GPIOC->MODER &= ~(0x3 << (A_POT_H * 2)); \
            GPIOC->MODER |= (ANALOG_MODE << (A_POT_H * 2)); \
            RCC->APB2ENR |= RCC_APB2ENR_ADC3EN; /* Enable ADC3 clock*/ \
            ADC3->SQR3 = 12; /*Select channel*/ \
            ADC3->SMPR2 = ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP1_1; /*Sample time 56 cycles*/\
            ADC3->CR2 = ADC_CR2_ADON /*Enable ADC*/; \
            } while(0)
     
        #define ENABLE_SYS_TICK(PRESCALER, COUNTER, interupt_priority) \
            /**************************************************
             * Enable SYSTICK
             * Description:
             *      Used to enable the SYSTICK timer and 
             *      SYSTICS hardware interupts
             * Inputs:
             *      COUNTER (uint32_t): the value to count
             *          up to. Only reads the first 24 bits
            **************************************************/ \
            do { \
            systick_prescaler = PRESCALER + 1; \
            SysTick->LOAD = COUNTER-1; /*Load value to count up to*/ \
            SysTick->VAL = 0; /* Clear current value */ \
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | \
                            SysTick_CTRL_TICKINT_Msk   | \
                            SysTick_CTRL_ENABLE_Msk; \
            NVIC_SetPriority(SysTick_IRQn, interupt_priority); 	/* Set the SysTick priority */ \
            } while(0)


        #define ENABLE_7SD(PRESCALER, PERIOD, interupt_priority) \
            /**************************************************
             * Enable 7 Segment Display
             * Description:
             *      Used to enable the pins for the 7 Segment
             *      display on the Trainor Brd hat. Also 
             *      configures a timer interput based on the 
             *      provided values to refresh the digits.
             * Inputs:
             *      PRESCALLER (uint16_t): the prescaller value
             *          used to configure the refresh timer
             *      PERIOD (uint32_t): the period used to 
             *          configure the timer 5 interupt for 
             *          screen refreshes
            **************************************************/ \
            do {\
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; /* Enable TIM5 clock*/ \
            TIM5->PSC = PRESCALER - 1; /* Prescaler*/ \
            TIM5->ARR = PERIOD - 1; /*Auto-reload when Count = period*/ \
            TIM5->DIER |= TIM_DIER_UIE; /*Enable update interrupt*/ \
            TIM5->SR &= ~TIM_SR_UIF; /*Clear any pending interrupt*/ \
            NVIC_EnableIRQ(TIM5_IRQn); /*Enable TIM5 interrupt in NVIC*/ \
            NVIC_SetPriority(TIM5_IRQn, interupt_priority); /*Set priority for TIM5*/ \
            /*Enables the GIPOs for the requiered pins*/ \
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOFEN; \
            /*Clear the MODE bits for all the Seven Seg Display pins*/ \
            GPIOB->MODER &= ~((0x3 << (SSD_DIGIT3_SEL * 2)) | \
                              (0x3 << (SSD_DIGIT4_SEL * 2)) | \
                              (0x3 << (SSD_G * 2)) ); \
            GPIOE->MODER &= ~((0x3 << (SSD_DIGIT1_SEL * 2)) | \
                              (0x3 << (SSD_DIGIT2_SEL * 2)) | \
                              (0x3 << (SSD_E * 2))); \
            GPIOF->MODER &= ~((0x3 << (SSD_B * 2)) | \
                              (0x3 << (SSD_C * 2)) | \
                              (0x3 << (SSD_F * 2)) | \
                              (0x3 << (SSD_DEC_PNT * 2))); \
            GPIOG->MODER &= ~((0x3 << (SSD_A * 2)) | \
                              (0x3 << (SSD_D * 2))); \
            /*Set the MODE bits for all the Seven Seg Display pins to OUTPUT*/ \
            GPIOB->MODER |= (OUTPUT_MODE << (SSD_DIGIT3_SEL * 2)) | \
                            (OUTPUT_MODE << (SSD_DIGIT4_SEL * 2)) | \
                            (OUTPUT_MODE << (SSD_G * 2)); \
            GPIOE->MODER |= (OUTPUT_MODE << (SSD_DIGIT1_SEL * 2)) | \
                            (OUTPUT_MODE << (SSD_DIGIT2_SEL * 2)) | \
                            (OUTPUT_MODE << (SSD_E * 2)); \
            GPIOF->MODER |= (OUTPUT_MODE << (SSD_B * 2)) | \
                            (OUTPUT_MODE << (SSD_C * 2)) | \
                            (OUTPUT_MODE << (SSD_F * 2)) | \
                            (OUTPUT_MODE << (SSD_DEC_PNT * 2)); \
            GPIOG->MODER |= (OUTPUT_MODE << (SSD_A * 2)) | \
                            (OUTPUT_MODE << (SSD_D *2)); \
            /*Set all the pins to high since the pins are common annode*/ \
            GPIOE->BSRR = (1U << SSD_DIGIT1_SEL) | (1U << SSD_DIGIT2_SEL); \
            GPIOB->BSRR = (1U << SSD_DIGIT3_SEL) | (1U << SSD_DIGIT4_SEL); \
            GPIOB->BSRR = (1U << SSD_G); \
            GPIOE->BSRR = (1U << SSD_E); \
            GPIOF->BSRR = (1U << SSD_B) | \
                          (1U << SSD_C) | \
                          (1U << SSD_F) | \
                          (1U << SSD_DEC_PNT); \
            GPIOG->BSRR = (1U << SSD_A) | (1U << SSD_D); \
            TIM5->CR1 = TIM_CR1_CEN; /*Enable TIM5*/ \
            } while(0)

        #define ENABLE_DAC_SPEAKER(lookup_table, table_size, BootFrequency) \
            /**************************************************
             * Enable Digital to Analog Converter Output
             * Description:
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
            **************************************************/ \
            do { \
            /*Enable DAC*/ \
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  /* Enable GPIOA */            \
                GPIOA->MODER |= (ANALOG_MODE << (DAC_H * 2)); /* Enables DAC function */  \
            /*Enables the conection between DMA and the DAC via channel 7*/ \
                RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; /*Enable DMA clock*/ \
                DMA1_Stream5->CR &= ~DMA_SxCR_EN; /*temp disable DMA stream configuring*/ \
                while (DMA1_Stream5->CR & DMA_SxCR_EN); /*Wait until the stream is disabled*/ \
                DMA1_Stream5->CR |= (7U << DMA_SxCR_CHSEL_Pos); /*Set DMA to rount over Stream 5 (REQUIRED FOR DAC1 by the manual)*/ \
                DMA1_Stream5->CR |= (0x1 << DMA_SxCR_MSIZE_Pos) | /*set the Data Sizes sent between the DAC and the Memory to 16bit*/ \
                                    (0x1 << DMA_SxCR_PSIZE_Pos); \
                DMA1_Stream5->CR |= DMA_SxCR_MINC; /*sets the pointer tick up the table*/ \
                DMA1_Stream5->CR |= (0b01 << DMA_SxCR_DIR_Pos); /* set the direction of the DMA: Memory-->DAC */ \
                DMA1_Stream5->CR |= DMA_SxCR_CIRC; /*Enable Circular Mode to wrap arround the table at the end*/ \
                DMA1_Stream5->NDTR = table_size; /*set the size of the lookup table*/ \
                DMA1_Stream5->PAR = (uint32_t)&(DAC->DHR12R1); /* set DAC1 to output data in a 12 bit Right-Aligned format*/ \
                DMA1_Stream5->M0AR = (uint32_t)lookup_table; /*set the pointer to the lookup table*/ \
                DMA1_Stream5->CR |= DMA_SxCR_EN; /*re-enable the DMA stream*/ \
            /*Enable the DAC*/ \
                RCC->APB1ENR |= RCC_APB1ENR_DACEN; \
            /*Configure the DAC to look for timer 6 as a trigger to go to the next LUT value*/ \
                DAC->CR &= ~DAC_CR_TSEL1; /*Select TIM6 TRGO as LUT shift Trigger */ \
                DAC->CR |= DAC_CR_TEN1; /*Enable Triggering for Channel 1*/ \
                DAC->CR |= DAC_CR_DMAEN1; /*Enable DMA Request Generation*/ \
                DAC->CR |= DAC_CR_EN1; /*Enable DAC Channel 1*/ \
            /*Config timer 6 as the DAC trigger source*/ \
                RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; \
                TIM6->CR1 &= ~TIM_CR1_CEN; /*Ensure TIM6 is disabled before configuring it*/\
                TIM6->PSC = 0; /*No prescaler (runs at full APB1 timer clock)*/ \
                TIM6->ARR = BootFrequency - 1; /*Auto-reload when Count = period*/ \
                /*Select Update Event as Trigger Output*/ \
                TIM6->CR2 &= ~TIM_CR2_MMS; \
                TIM6->CR2 |= (0x2 << TIM_CR2_MMS_Pos);\
                TIM6->CR1 |= TIM_CR1_CEN; /*Enable timer 6*/ \
            /*Enable the the speaker*/ \
                RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  /* Enable the speaker AMP */  \
                GPIOB->MODER &= ~(3U << (AUDIO_SD_H * 2)); /*Clear PB6*/ \
                GPIOB->MODER |= (1U << (AUDIO_SD_H * 2)); /*Set PB6 to OUTPUT Mode*/ \
                GPIOB->BSRR = (1U << AUDIO_SD_H); /*Set PB6 high to enable audio amplifier*/ \
            } while(0)

        #define ENABLE180MHZCLOCK() \
            /**************************************************
             * Enable 180MHz Clock speeds
             * Description:
             *      Used to set up the board to switch from 
             *      the base 16Mhz to the max board speed of 
             *      180 MHz
            **************************************************/ \
            do { \
            RCC->CR |= RCC_CR_HSION; /* Enable HSI (default with no HSE)*/ \
            while (!(RCC->CR & RCC_CR_HSIRDY)); /* Wait until HSI is ready */ \
            /*Configure PLL: 
                VCO = (16MHz / M) * N
                SYSCLK = VCO / P = 180MHz, 
                USB OTG FS, SDIO and RNG Clock = VCO / Q = 180MHz 
            */ \
            RCC->PLLCFGR = (8ul) | (180ul << 6) | (0ul << 16) | (2ul << 24); /*sets the values for the pll*/\
            RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLSRC; /* Set HSI as PLL source */ \
            \
            /*
              Set prescalers: 
                AHB=SYSCLK/1, bc all AHB has a max clock of 180 MHz
                APB2=AHB/2, bc APB2 has a max clock of 90MHz
                APB1=AHB/4, bc APB1 has a max clock of 45Mhz
            */ \
            RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_PPRE1_DIV4;\
            \
            RCC->CR |= RCC_CR_PLLON; /*Enable the PLL*/ \
            while (!(RCC->CR & RCC_CR_PLLRDY)); /* Wait until PLL is finished syncronizing to 180Mhz */ \
            /*
            The system power settings must be tweaked to allow for the clock
            to run at the full 180Mhz
            */ \
            RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* Enable power interface clock */ \
            PWR->CR |= PWR_CR_ODEN; /*Enable Over-drive mode*/ \
            while (!(PWR->CSR & PWR_CSR_ODRDY)); /*Wait until Over-drive mode is ready*/ \
            PWR->CR |= PWR_CR_ODSWEN; /*Enable Over-drive switching*/ \
            while (!(PWR->CSR & PWR_CSR_ODSWRDY)) ; /*Wait until Over-drive switching is ready*/ \
            \
            /*
            The memory profile must be tweaked to increase the number of cyles between reads and writes from one to five
            */\
            FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS; /*Set flash latency to 5 wait states*/ \
            /*
            Officialy switches over the clock from the HSI to the PLL
            */ \
            RCC->CFGR &=~ RCC_CFGR_SW; /*Clear SW bits*/ \
            RCC->CFGR |= RCC_CFGR_SW_PLL; /*Select PLL as system clock source*/ \
            while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL); /*Wait until PLL is used as system clock source*/ \
            } while(0)

        // Functions
            static inline void ssd_display_digit(uint8_t digit, uint8_t number, bool dec_pnt) {
                /******************************************
                 * SSD Display Digit
                 * Description:
                 *      Used to display digits 0-9 on the
                 *      Trainer Board hat
                ******************************************/
                // Set all the digits to high (off) to prevent ghosting
                GPIOE->BSRR = (1U << SSD_DIGIT1_SEL) | (1U << SSD_DIGIT2_SEL);
                GPIOB->BSRR = (1U << SSD_DIGIT3_SEL) | (1U << SSD_DIGIT4_SEL);
                
                // Grab the sequence based on the number being displayed
                uint8_t segment_bits = ssddigitSegments[number];

                // Set the segmets to low bases on what needs to be displayed
                GPIOB->BSRR = (1U << (SSD_G + (((segment_bits >> 6) & 1U) ? 16U : 0)));
                GPIOE->BSRR = (1U << (SSD_E + (((segment_bits >> 4) & 1U) ? 16U : 0)));
                GPIOF->BSRR = (1U << (SSD_B + (((segment_bits >> 1) & 1U) ? 16U : 0))) | 
                            (1U << (SSD_C + (((segment_bits >> 2) & 1U) ? 16U : 0))) | 
                            (1U << (SSD_F + (((segment_bits >> 5) & 1U) ? 16U : 0))) | 
                            (1U << (SSD_DEC_PNT + ((dec_pnt==true) ? 16U : 0)));
                GPIOG->BSRR = (1U << (SSD_A + ((segment_bits & 1U) ? 16U : 0))) | 
                            (1U << (SSD_D + (((segment_bits >> 3) & 1U) ? 16U : 0)));


                // Set the digit that needs to be enabled back to low (on)
                switch (digit)
                {
                case 3: GPIOE->BSRR = (1U << (SSD_DIGIT1_SEL + 16U)); break;
                case 2: GPIOE->BSRR = (1U << (SSD_DIGIT2_SEL + 16U)); break;
                case 1: GPIOB->BSRR = (1U << (SSD_DIGIT3_SEL + 16U)); break;
                case 0: GPIOB->BSRR = (1U << (SSD_DIGIT4_SEL + 16U)); break;
                }        
            }
            static inline void read_Hat_POT(uint32_t *total) {
                uint32_t temp_total = 0U;
                for(int i = 0; i < 16; i++){
                    ADC3->CR2 |= ADC_CR2_SWSTART; // Start conversion
                    while(!(ADC3->SR & ADC_SR_EOC)); // Wait for conversion to complete
                    temp_total += ADC3->DR; // Read data
                }
                *total = (uint32_t)(temp_total / 16);
            }
        
        // "Functions" (One line define macros)
        #define speaker_set_frequency(frequency) TIM6->ARR = frequency - 1;
        #define speaker_set_amplitude(amplitude) DAC->DHR12R1 = amplitude;
        #define read_switch(switch_v) ((GPIOC->IDR & (1 << switch_v)) != 0)
#endif