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
#ifndef ENABLE_HARDWARE
#define ENABLE_HARDWARE
    #define Enabled true
    #define Disabled false
    /// Functions:
        void Enable_Hardware();
    #define DEMO_number 10
    // Enabled Features
    #define UART                        Disabled
    #define DEBUG_MODE                  Disabled
    #define OLED                        Enabled
    #define SWITCHES                    Disabled
    #define BUTTONS                     Enabled
    #define BUTTON_INTERUPTS            Enabled
        #define BUTTON_INTRUPT_PRIORITY     2
    #define SSD                         Disabled
    #define NEOPIXELS                   Disabled
        #define NEOPIXEL_RED_BOOT           0
        #define NEOPIXEL_GREEN_BOOT         0
        #define NEOPIXEL_BLUE_BOOT          0
    #define SHIELD_LEDS                 Enabled
    #define SYSTICK                     Enabled
        #define SYSTICK_DIVISOR             18000000U        
        #define SYSTIC_INTERUPT_PRIORITY    1
    #define AUDIO_DAC                   Enabled
    #define I2C                         Disabled
        #define BME280                      Disabled
    #define POTENTIOMITER               Disabled
    #define MOTORS                      Enabled    

#endif