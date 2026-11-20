/**********************************************************************
 * Name: Enable Hardware
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/12/2026 (mm/dd/yyyy)
 * Creation Date: 11/12/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Used to enable the neccesary parts of each project
**********************************************************************/
#include <stm32f446xx.h>
#include "unit_values.h"
#include "logo_bitmap.h"

#define DebugMode true
// Src
    #include "cpeg222_driver.c"
// Libs
    #include "uart_driver.h"
    #include "oled_driver.h"
    #include "pwm_motor_driver.h"
    #include "enable_hardware.h"
    #define UART_DEBUG_EN DEBUG_MODE && UART

void Enable_Hardware() {
    // Hardware Settup
        //clock
            RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
            ENABLE180MHZCLOCK(); // Start up 180MHz clock and switch to it
        // Serial Debug
            #if UART
                ENABLE_UART();
                delay_ms(2000); // Give Debug serial a chance to connect
                    #if DebugMode
                    serial_printf("-------------------\n");
                    serial_printf("CPEG222 Demo %d\n", DEMO_number);
                    serial_printf("Fall 2026\n");
                    serial_printf("Brandon Trainor\n");
                    serial_printf("-------------------\n\n");
                    #endif
            #endif
        // Oled
            #if OLED
                ENABLE_OLED();
                OLED_Render_Screen_Buffer(UD_logo_128x64);
                #if UART_DEBUG_EN
                    serial_printf("Enabled OLED\n");
                #endif
            #endif
        // Inputs
            #if SWITCHES
            ENABLE_SHIELD_SWITCHES();
                #if UART_DEBUG_EN
                    serial_printf("Enabled Switches\n", 18);
                #endif 
            #endif
            #if BUTTONS
                ENABLE_SHIELD_BUTTONS();
                #if UART_DEBUG_EN
                    serial_printf("Enabled Buttons\n");
                #endif
            #endif
            #if BUTTON_INTERUPTS
                ENABLE_SHIELD_BUTTON_INTERUPTS(BUTTON_INTRUPT_PRIORITY);
                #if UART_DEBUG_EN
                    serial_printf("Enabled Button Interupts\n");
                #endif
            #endif
        // Other Displays
            #if SSD
                ENABLE_SSD(45,1000,3);
                #if UART_DEBUG_EN
                    serial_printf("Enabled SSD\n");
                #endif
            #endif
            #if NEOPIXELS
                ENABLE_NEOPIXELS();
                NeoPixel_Display_Solid_Color(NEOPIXEL_RED_BOOT,NEOPIXEL_GREEN_BOOT,NEOPIXEL_BLUE_BOOT);
                    #if UART_DEBUG_EN
                        serial_printf("Enabled NeoPixels\n", 19);
                    #endif
            #endif
            #if SHIELD_LEDS
                ENABLE_SHIELD_LEDS();
                #if UART_DEBUG_EN
                serial_printf("Enabled Hat LEDs\n", 18);
                #endif
            #endif
        // Systic Timer
            #if SYSTICK
                SystemCoreClockUpdate(); 
                ENABLE_SYS_TICK((SystemCoreClock / SYSTICK_DIVISOR), SYSTIC_INTERUPT_PRIORITY);
                    #if UART_DEBUG_EN
                        serial_printf("Enabled Systick\n");
                    #endif
            #endif
        // // Audio
            #if AUDIO_DAC
                ENABLE_DAC_SPEAKER(C4);
                    #if UART_DEBUG_EN
                    serial_printf("Enabled DAC\n", 12);
                    #endif
            #endif
        // // I2C
        #if I2C
            Enable_I2C();
            #if UART_DEBUG_EN
            serial_printf("Enabled I2C\n", 12);
            #endif
            #if BME280
                Enable_BME280();
                #if UART_DEBUG_EN
                serial_printf("Enabled BME280\n", 16);
                #endif
            #endif
        #endif
        // Other
            #if POTENTIOMITER
                ENABLE_POT_INPUT();
               #if UART_DEBUG_EN
                serial_printf("Enabled the Potentiomiter\n", 27);
               #endif
            #endif
        // Motors
            #if MOTORS
                ENABLE_MOTORS();
                    #if UART_DEBUG_EN
                        serial_printf("Enabled Motors\n", 16);
                    #endif
            #endif
        // Other

        //Final Steps
            #if OLED
                delay_ms(2000);
                OLED_Clear();
            #endif
}