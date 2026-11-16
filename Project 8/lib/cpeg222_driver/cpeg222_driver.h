/************************************************************************
 * Name: CPEG222 Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/22/2026 (mm/dd/yyyy)
 * Creation Date: 9/24/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
    * CPEG222 Robot Base Rev.C
 * Description:
    * Pins, startup macros, and common functions used for CPEG222 @ UDEL
 * Attribution:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
************************************************************************/
#ifndef CPEG222_DRIVER
#define CPEG222_DRIVER
#include "stm32f446xx.h"
#include <stdbool.h>
    // Constant Values
        const unsigned char ssddigitSegments[11];
        const uint16_t place[4];
        static volatile uint8_t digit_selector = 0;
    // Pin Mode Values
        #define INPUT_MODE      0b00
        #define OUTPUT_MODE     0b01
        #define ALT_FUNC_MODE   0b10
        #define ANALOG_MODE     0b11

    // Board Pins
        //LEDS
            #define B_GREEN_LED 0
            #define B_BLUE_LED 7
            #define B_RED_LED 14
            #define B_LED_GPIO GPIOB

        // Board(User) Button Stuff
            #define B_BTN_PIN 13
            #define B_USR_BTN_PRESSED ((GPIOC->IDR & (1<<BTN_PIN)) != 0)

    // CPEG222 Sheild Pins
        // LEDS (GPIOD)
            #define LED_HAT_GPIO    GPIOD
            #define LED1_H          0
            #define LED2_H          1
            #define LED3_H          2
            #define LED4_H          3
            #define LED5_H          4
            #define LED6_H          5
            #define LED7_H          6
            #define LED8_H          7
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
        // I2C Pins
            #define I2C_SDA     8
            #define I2C_SCL     9
            #define I2C_PINS_GPIO    GPIOB
            #define I2C_PINS_GPIOEN  RCC_AHB1ENR_GPIOBEN
            #define I2C_ENABLE       RCC_APB1ENR_I2C1EN
    // Settup Macros 
        static inline void ENABLE_SHIELD_BUTTONS(void) {
            /******************************************
             * Enable Shield Buttons
             * Description:
             *      Used to set the enable registers 
             *      to input for all 5 of the buttons
             *      on the Trainer Brd Hat
            ******************************************/ 
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN; 
            GPIOE->MODER &= ~((0x3 << (BTN_DOWN_H * 2)) | (0x3 << (BTN_RIGHT_H * 2))); 
            GPIOF->MODER &= ~((0x3 << (BTN_UP_H * 2)) | (0x3 << (BTN_LEFT_H * 2)) | (0x3 << (BTN_CENTER_H * 2))); 
            GPIOE->MODER |= (INPUT_MODE << (BTN_DOWN_H * 2)) + (INPUT_MODE << (BTN_RIGHT_H * 2)); 
            GPIOF->MODER |= (INPUT_MODE << (BTN_UP_H * 2)) | (INPUT_MODE << (BTN_CENTER_H * 2)) | (INPUT_MODE << (BTN_LEFT_H * 2)); 
        }
        
        static inline void ENABLE_SHIELD_LEDS(void) {
            /******************************************
             * Enable Shield LEDs
             * Description:
             *      Used to set the enable registers 
             *      to input for all 8 of the red leds
             *      on the Trainer Brd Hat
            ******************************************/ 
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; 
            GPIOD->MODER &= ~((0x3 << (LED1_H * 2)) | (0x3 << (LED2_H * 2)) | 
                                (0x3 << (LED3_H * 2)) | (0x3 << (LED4_H * 2)) | 
                                (0x3 << (LED5_H * 2)) | (0x3 << (LED6_H * 2)) | 
                                (0x3 << (LED7_H * 2)) | (0x3 << (LED8_H * 2))); 
            GPIOD->MODER |= (OUTPUT_MODE << (LED1_H * 2)) | (OUTPUT_MODE << (LED2_H * 2)) | 
                            (OUTPUT_MODE << (LED3_H * 2)) | (OUTPUT_MODE << (LED4_H * 2)) | 
                            (OUTPUT_MODE << (LED5_H * 2)) | (OUTPUT_MODE << (LED6_H * 2)) | 
                            (OUTPUT_MODE << (LED7_H * 2)) | (OUTPUT_MODE << (LED8_H * 2));  
        }

        static inline void ENABLE_SHIELD_BUTTON_INTERUPTS(const int button_interupt_priority) {
            /****************************************************
             * Enable Shield Button Interupts
             * Description:
             *      Used to enable hardware interupts for all 5 
             *      of the buttons on the Trainer Brd Hat. 
             *      Interupt handler must be named 
             *      EXTI9_5_IRQHandler and at a minimum detect 
             *      and clear the flags for all five buttons
             * Inputs:
             *      button_interupt_priority(int 1 through 24): 
             *              sets the interupt priority for the 
             *              interupt handler 
            ****************************************************/ 
            SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI5 | SYSCFG_EXTICR2_EXTI6 | SYSCFG_EXTICR2_EXTI7); 
            SYSCFG->EXTICR[2] &= ~(SYSCFG_EXTICR3_EXTI8 | SYSCFG_EXTICR3_EXTI9); 
            SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PE | SYSCFG_EXTICR2_EXTI6_PE | SYSCFG_EXTICR2_EXTI7_PF; 
            SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PF | SYSCFG_EXTICR3_EXTI9_PF; 
            EXTI->IMR |= (1 << BTN_UP_H) | (1 << BTN_DOWN_H) | (1 << BTN_LEFT_H) | (1 << BTN_RIGHT_H) | (1 << BTN_CENTER_H); 
            EXTI->RTSR |= (1 << BTN_UP_H) | (1 << BTN_DOWN_H) | (1 << BTN_LEFT_H) | (1 << BTN_RIGHT_H) | (1 << BTN_CENTER_H); 
            NVIC_SetPriority(EXTI9_5_IRQn, button_interupt_priority); 
            NVIC_EnableIRQ(EXTI9_5_IRQn); 
        }

        static inline void ENABLE_SHIELD_SWITCHES(void) {
            /******************************************
             * Enable Shield Switches
             * Description:
             *      Used to set the enable registers 
             *      to input for all 4 of the switches
             *      on the Trainer Brd Hat
            ******************************************/ 
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; 
            GPIOC->MODER &= ~((0x3 << (SW1_H * 2)) | (0x3 << (SW2_H * 2)) | (0x3 << (SW3_H * 2)) |(0x3 << (SW4_H * 2))); 
            GPIOC->MODER |= (INPUT_MODE << (SW1_H * 2)) | (INPUT_MODE << (SW2_H * 2)) | 
                            (INPUT_MODE << (SW3_H * 2)) | (INPUT_MODE << (SW4_H * 2));
        }

        static inline void ENABLE_POT_INPUT(void) {
            /******************************************
             * Enable Potentiomiter Input
             * Description:
             *      Used to set the enable registers 
             *      to analog input for the Volume
             *      potentiomiter and configure the 
             *      Analog to Digital Converter
            ******************************************/ 
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; 
            GPIOC->MODER &= ~(0x3 << (A_POT_H * 2)); 
            GPIOC->MODER |= (ANALOG_MODE << (A_POT_H * 2)); 
            RCC->APB2ENR |= RCC_APB2ENR_ADC3EN; // Enable ADC3 clock
            ADC3->SQR3 = 12; // Select channel 
            ADC3->SMPR2 = ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP1_1; //Sample time 56 cycles
            ADC3->CR2 = ADC_CR2_ADON; //Enable ADC 
        }
        
        static inline void ENABLE_SYS_TICK(uint32_t COUNTER, uint8_t systick_interupt_priority) {
            /**************************************************
             * Enable SYSTICK
             * Description:
             *      Used to enable the SYSTICK timer and 
             *      SYSTICS hardware interupts
             * Inputs:
             *      COUNTER (uint32_t): the value to count
             *          up to. Only reads the first 24 bits
            **************************************************/ 
            SysTick->LOAD = COUNTER-1; /*Load value to count up to*/ 
            SysTick->VAL = 0; /* Clear current value */ 
            SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
                            SysTick_CTRL_TICKINT_Msk   | 
                            SysTick_CTRL_ENABLE_Msk; 
            NVIC_SetPriority(SysTick_IRQn, systick_interupt_priority); 	/* Set the SysTick priority */ 
        }

        static inline void ENABLE_SSD(uint32_t PRESCALER, uint32_t PERIOD, uint8_t ssd_interupt_priority) {
            /**************************************************
             * Enable Seven Segment Display
             * Description:
             *      Used to enable the pins for the Seven Segment
             *      display on the Trainor Brd hat. Also 
             *      configures a timer interput based on the 
             *      provided values to refresh the digits.
             * Inputs:
             *      PRESCALLER (uint16_t): the prescaller value
             *          used to configure the refresh timer
             *      PERIOD (uint32_t): the period used to 
             *          configure the timer 5 interupt for 
             *          screen refreshes
            **************************************************/ 
            RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; /* Enable TIM5 clock*/ 
            TIM5->PSC = PRESCALER - 1; /* Prescaler*/ 
            TIM5->ARR = PERIOD - 1; /*Auto-reload when Count = period*/ 
            TIM5->DIER |= TIM_DIER_UIE; /*Enable update interrupt*/ 
            TIM5->SR &= ~TIM_SR_UIF; /*Clear any pending interrupt*/ 
            NVIC_EnableIRQ(TIM5_IRQn); /*Enable TIM5 interrupt in NVIC*/ 
            NVIC_SetPriority(TIM5_IRQn, ssd_interupt_priority); /*Set priority for TIM5*/ 
            /*Enables the GIPOs for the requiered pins*/ 
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOFEN; 
            /*Clear the MODE bits for all the Seven Seg Display pins*/ 
            GPIOB->MODER &= ~((0x3 << (SSD_DIGIT3_SEL * 2)) | 
                                (0x3 << (SSD_DIGIT4_SEL * 2)) | 
                                (0x3 << (SSD_G * 2)) ); 
            GPIOE->MODER &= ~((0x3 << (SSD_DIGIT1_SEL * 2)) | 
                                (0x3 << (SSD_DIGIT2_SEL * 2)) | 
                                (0x3 << (SSD_E * 2))); 
            GPIOF->MODER &= ~((0x3 << (SSD_B * 2)) | 
                                (0x3 << (SSD_C * 2)) | 
                                (0x3 << (SSD_F * 2)) | 
                                (0x3 << (SSD_DEC_PNT * 2))); 
            GPIOG->MODER &= ~((0x3 << (SSD_A * 2)) | 
                                (0x3 << (SSD_D * 2))); 
            /*Set the MODE bits for all the Seven Seg Display pins to OUTPUT*/ 
            GPIOB->MODER |= (OUTPUT_MODE << (SSD_DIGIT3_SEL * 2)) | 
                            (OUTPUT_MODE << (SSD_DIGIT4_SEL * 2)) | 
                            (OUTPUT_MODE << (SSD_G * 2)); 
            GPIOE->MODER |= (OUTPUT_MODE << (SSD_DIGIT1_SEL * 2)) | 
                            (OUTPUT_MODE << (SSD_DIGIT2_SEL * 2)) | 
                            (OUTPUT_MODE << (SSD_E * 2)); 
            GPIOF->MODER |= (OUTPUT_MODE << (SSD_B * 2)) | 
                            (OUTPUT_MODE << (SSD_C * 2)) | 
                            (OUTPUT_MODE << (SSD_F * 2)) | 
                            (OUTPUT_MODE << (SSD_DEC_PNT * 2)); 
            GPIOG->MODER |= (OUTPUT_MODE << (SSD_A * 2)) | 
                            (OUTPUT_MODE << (SSD_D *2)); 
            /*Set all the pins to high since the pins are common annode*/ 
            GPIOE->BSRR = (1U << SSD_DIGIT1_SEL) | (1U << SSD_DIGIT2_SEL); 
            GPIOB->BSRR = (1U << SSD_DIGIT3_SEL) | (1U << SSD_DIGIT4_SEL); 
            GPIOB->BSRR = (1U << SSD_G); 
            GPIOE->BSRR = (1U << SSD_E); 
            GPIOF->BSRR = (1U << SSD_B) | 
                            (1U << SSD_C) | 
                            (1U << SSD_F) | 
                            (1U << SSD_DEC_PNT); 
            GPIOG->BSRR = (1U << SSD_A) | (1U << SSD_D); 
            TIM5->CR1 = TIM_CR1_CEN; /*Enable TIM5*/ 
        }

        static inline void ENABLE180MHZCLOCK(void) {
            /**************************************************
             * Enable 180MHz Clock speeds
             * Description:
             *      Used to set up the board to switch from 
             *      the base 16Mhz to the max board speed of 
             *      180 MHz
            **************************************************/ 
            RCC->CR |= RCC_CR_HSION; /* Enable HSI (default with no HSE)*/ 
            while (!(RCC->CR & RCC_CR_HSIRDY)); /* Wait until HSI is ready */ 
            /*Configure PLL: 
                VCO = (16MHz / M) * N
                SYSCLK = VCO / P = 180MHz, 
                USB OTG FS, SDIO and RNG Clock = VCO / Q = 180MHz 
            */ 
            RCC->PLLCFGR = (8ul) | (180ul << 6) | (0ul << 16) | (2ul << 24); /*sets the values for the pll*/
            RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLSRC; /* Set HSI as PLL source */ 
            
            /*
                Set prescalers: 
                AHB=SYSCLK/1, bc all AHB has a max clock of 180 MHz
                APB2=AHB/2, bc APB2 has a max clock of 90MHz
                APB1=AHB/4, bc APB1 has a max clock of 45Mhz
            */ 
            RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_PPRE1_DIV4; /*Config Prescalers*/ 
            RCC->CR |= RCC_CR_PLLON; /*Enable the PLL*/ 
            while (!(RCC->CR & RCC_CR_PLLRDY)); /* Wait until PLL is finished syncronizing to 180Mhz */ 
            /*
            The system power settings must be tweaked to allow for the clock
            to run at the full 180Mhz
            */ 
            RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* Enable power interface clock */ 
            PWR->CR |= PWR_CR_ODEN; /*Enable Over-drive mode*/ 
            while (!(PWR->CSR & PWR_CSR_ODRDY)); /*Wait until Over-drive mode is ready*/ 
            PWR->CR |= PWR_CR_ODSWEN; /*Enable Over-drive switching*/ 
            while (!(PWR->CSR & PWR_CSR_ODSWRDY)) ; /*Wait until Over-drive switching is ready*/ 
            
            /*
            The memory profile must be tweaked to increase the number of cyles between reads and writes from one to five
            */
            FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS; /*Set flash latency to 5 wait states*/ 
            /*
            Officialy switches over the clock from the HSI to the PLL
            */ 
            RCC->CFGR &=~ RCC_CFGR_SW; /*Clear SW bits*/ 
            RCC->CFGR |= RCC_CFGR_SW_PLL; /*Select PLL as system clock source*/ 
            while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL); /*Wait until PLL is used as system clock source*/
            SystemCoreClockUpdate(); // Update SystemCoreClock variable after clock change
        }

        static inline void FPU_Enable(void) {
            // Enable CP10 and CP11 coprocessors (Full Access = 0b11)
            // SCB->CPACR bits [23:22] for CP11 and bits [21:20] for CP10
            SCB->CPACR |= ((3UL << (10 * 2)) | (3UL << (11 * 2)));
            // Memory barrier to ensure FPU initialization completes before next instruction
            __DSB();
            __ISB();
        }

    // Inline Macros
        static inline void read_Hat_POT(uint32_t *total) {
            uint32_t temp_total = 0U;
            for(int i = 0; i < 16; i++){
                ADC3->CR2 |= ADC_CR2_SWSTART; // Start conversion
                while(!(ADC3->SR & ADC_SR_EOC)); // Wait for conversion to complete
                temp_total += ADC3->DR; // Read data
            }
            *total = (uint32_t)(temp_total / 16);
        }

        static inline bool read_switch(int switch_pin) {
            return ((GPIOC->IDR & (1 << switch_pin))  != 0);
        }

        static inline void ssd_interupt_controller(uint16_t displayed_value, uint8_t precision){
            int display_num = (displayed_value/place[digit_selector]) % 10;
            if (display_num == 0 && digit_selector == 3) {
                display_num = 10;
            }
            ssd_display_digit(digit_selector, display_num, (digit_selector == precision ? 1 : 0));
            digit_selector = (digit_selector + 1) % 4; // Cycle through digitSelect values 0 to 3
        }

    // Function Headers
        void delay_ms(uint32_t);
        void ssd_display_digit(uint8_t digit, uint8_t number, bool dec_pnt);
        void hat_led_display_digits(uint8_t lights);
    // Settup Defines
#endif