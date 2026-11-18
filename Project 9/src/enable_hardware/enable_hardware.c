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
#include "compile_settings.h"
// Src
    #include "cpeg222_driver.c"
// Libs
    #include "uart_driver.h"
    #include "oled_driver.h"
    #include "pwm_motor_driver.h"
    #define DEMO_number 9

void Enable_Hardware() {
    // Hardware Settup
        //clock
            RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
            ENABLE180MHZCLOCK(); // Start up 180MHz clock and switch to it
        // Serial Debug
            ENABLE_UART();
            delay_ms(1000); // Give Debug serial a chance to connect
            serial_printf("-------------------\n", DEMO_number);
            serial_printf("CPEG222 Demo %d\n", DEMO_number);
            serial_printf("Fall 2027\n");
            serial_printf("Brandon Trainor\n");
            serial_printf("-------------------\n\n", DEMO_number);
        // Oled
            ENABLE_OLED();
            #if DebugMode
                serial_printf("Enabled OLED\n");
            #endif
            OLED_Render_Screen_Buffer(UD_logo_128x64);
        // Inputs
            // ENABLE_SHIELD_SWITCHES();
            // serial_printf("Enabled Switches\n", 18);
            ENABLE_SHIELD_BUTTONS();
            #if DebugMode
                serial_printf("Enabled Buttons\n");
            #endif
            ENABLE_SHIELD_BUTTON_INTERUPTS(8);
            #if DebugMode
                serial_printf("Enabled Button Interupts\n");
            #endif
        // Other Displays
            ENABLE_SSD(45,1000,3);
            #if DebugMode
                serial_printf("Enabled SSD\n");
            #endif
            // ENABLE_NEOPIXELS();
            // NeoPixel_Display_Solid_Color(0,0,0);
            // serial_printf("Enabled NeoPixels\n", 19);
            // ENABLE_SHIELD_LEDS();
            // serial_printf("Enabled Hat LEDs\n", 18);
        // Systic Timer
            ENABLE_SYS_TICK(180000U, 3);
            #if DebugMode
                serial_printf("Enabled Systick\n");
            #endif
        // // Audio
        //     ENABLE_DAC_SPEAKER(C4);
        //     serial_printf("Enabled DAC\n", 12);
        // // I2C
        //     Enable_I2C();
        //     #if DebugMode
        //     serial_printf("Enabled I2C\n", 12);
        //     Enable_BME280();
        //     #if DebugMode
        //     serial_printf("Enabled BME280\n", 16);
        // Other
            ENABLE_POT_INPUT();
            #if DebugMode
            #endif
        // Motors
            ENABLE_MOTORS();
            #if DebugMode
            #endif
        // Other
            delay_ms(2000);
            OLED_Clear();
}