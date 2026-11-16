/**********************************************************************
 * Name: Project 8
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/07/2026 (mm/dd/yyyy)
 * Creation Date: 10/06/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * BME280 Humidity, Temp, and pressure sensor weather station over I2C 
**********************************************************************/
// C Header Files
#include <stdbool.h>
#include <stdio.h>

// External Header Files
#include <stm32f4xx.h>

// Include Header Files
#include "unit_values.h" //Personal header file with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
#include "logo_bitmap.h" //Bootup Logo Bitmap
#include "speaker_driver.h"
#include "neopixel_driver.h"

// Lib Header Files
#include "cpeg222_driver.h" // Personal header with common pin names for the NANO F446ZE board
#include "oled_driver.h"
#include "uart_driver.h"
#include "ultrasonic_driver.h"
#include "i2c_driver.h"
#include "bme280_driver.h"

// Defines
    #define SSD_Precision 2
//Function Headers

// Vars
    // Bools
        volatile bool update_flag = false;
        bool lights_toggle = true;
        bool audio_toggle = true;
        bool alarm_toggle = false; 
        bool flashflip = false;
    // Ints
        volatile unsigned int time_count = 0;
    // Chars
        char buffer0[25] = {0};
        char buffer1[25] = {0};
        char buffer2[25] = {0};
    // uints
        volatile uint16_t ssd_value;
                 uint8_t displayed_neopixel_color=4;
    // floats 
        
// Constants
// Handlers



void TIM5_IRQHandler(void){
    if(TIM5->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
        ssd_interupt_controller(ssd_value, SSD_Precision);
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }
}

void SysTick_Handler(void) {
    if (time_count >= 1000 && !update_flag) {
        update_flag = true;
        time_count = 0;
    }
    else if (!update_flag) {
        time_count++;
    };
}

// Functions
void Enable_Hardware() {
    // Hardware Settup
        //clock
            RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
            ENABLE180MHZCLOCK();
        // Serial Debug
            ENABLE_UART();
            delay_ms(2000); // Give Debug serial a chance to connect
        // Oled
            ENABLE_OLED();
            serial_printf("Enabled OLED\n", 14);
            OLED_Render_Screen_Buffer(UD_logo_128x64);
        // Inputs
            ENABLE_SHIELD_SWITCHES();
            serial_printf("Enabled Switches\n", 18);
            ENABLE_SHIELD_BUTTONS();
            ENABLE_SHIELD_BUTTON_INTERUPTS(2);
        // Other Displays
            ENABLE_SSD(45,1000,3);
            serial_printf("Enabled SSD\n", 13);
            ENABLE_NEOPIXELS();
            NeoPixel_Display_Solid_Color(0,0,0);
            serial_printf("Enabled NeoPixels\n", 19);
            ENABLE_SHIELD_LEDS();
            serial_printf("Enabled Hat LEDs\n", 18);
        // Systic Timer
            ENABLE_SYS_TICK(180000U, 3);
            serial_printf("Enabled Systick\n", 16);
        // Audio
            ENABLE_DAC_SPEAKER(C4);
            serial_printf("Enabled DAC\n", 12);
        // // I2C
            Enable_I2C();
            serial_printf("Enabled I2C\n", 12);
            Enable_BME280();
            serial_printf("Enabled BME280\n", 16);
            BME280_Read_Data();
            serial_printf("Read BME280 Data\n", 18);
            serial_printf("Is the BME Working?:\n", 21);
            char startup_text[100] = {};
            sprintf(startup_text, "Temp: %i, Humidity: %i, Pressure:%i\n", (int)g_temperature, (int)g_humidity, (int)g_pressure);
            serial_printf(startup_text,60);
            delay_ms(2000);
            OLED_Clear();
}

void update_displays(bool unit) {
    if (!unit) { // if in deg F
        ssd_value = (((g_temperature * 9) / 5) + 3200);
        snprintf(buffer0, sizeof(buffer0), "Temp: %0.2f F", (float)(ssd_value*0.01f));
    }
    else { // if in deg C
        ssd_value = g_temperature;
        snprintf(buffer0, sizeof(buffer0), "Temp: %0.2f C", (float)(g_temperature*0.01f));
    }
    snprintf(buffer1, sizeof(buffer1), "Pressure: %0.2f hPa", (float)(g_pressure*0.01f));
    snprintf(buffer2, sizeof(buffer2), "Humidity: %0.3f%% RH", (float)(g_humidity*0.001));
    OLED_Print(0,0,"CPEG222 F26 Project 8");
    OLED_Print(1,0,"Brandon Trainor");
    OLED_Print(3,0,buffer0);
    OLED_Print(5,0,buffer1);
    OLED_Print(7,0,buffer2);
    
}

int main() {

    Enable_Hardware();
    update_flag = true;
    bool display_flag = true;
    bool current_unit = false; //false is deg C, true is deg F
    while(1) {
        // Check for unit swap
        if (read_switch(SW1_H) != current_unit) {
            current_unit = !current_unit;
            display_flag = true;
        }
        lights_toggle = !read_switch(SW2_H); // saves my corneas
        audio_toggle = !read_switch(SW3_H); // saves my ears
        alarm_toggle = !read_switch(SW4_H); // saves my sanity

        uint8_t current_neopixel_color = 4;
        if (update_flag){
            // Get New Data from the BME280
            BME280_Read_Data();
            if (g_temperature < 2111) {
                current_neopixel_color = 0;
            } 
            else if ((g_temperature >= 2111) && (g_temperature <= 2666 )) {
                current_neopixel_color = 1;
            }
            else current_neopixel_color = 2;

            update_flag = false;
            display_flag = true;
            flashflip = !flashflip;
        }
            // NeoPixel Update Check
            if (lights_toggle) {
                if ((g_temperature >= 2666) || (g_humidity > 50000)){
                    NeoPixel_SetColor(0,flashflip ? 64:0,0,flashflip ? 0:64);
                    NeoPixel_SetColor(1,flashflip ? 0:64,0,flashflip ? 64:0);
                    NeoPixel_SetColor(2,flashflip ? 64:0,0,flashflip ? 0:64);
                    NeoPixel_SetColor(3,flashflip ? 0:64,0,flashflip ? 64:0);
                    NeoPixel_Show();
                    speaker_set_frequency(flashflip ? F4_SH:C4)
                    enable_speaker();
                }
                else {
                    disable_speaker();
                    if (current_neopixel_color != displayed_neopixel_color)
                    {
                        displayed_neopixel_color = current_neopixel_color;
                        switch (displayed_neopixel_color)
                        {
                        case 0:
                            NeoPixel_Display_Solid_Color(0, 0, 64);
                            break;
                        case 1:
                            NeoPixel_Display_Solid_Color(32, 32, 0);
                            break;
                        case 2:
                            NeoPixel_Display_Solid_Color(64, 0, 0);
                            break;
                        default:
                            break;
                        }
                    }
                }

                int led_count = (uint8_t)(g_humidity / 12500);
                uint8_t digits = 0;
                switch (led_count)
                {
                case 1:
                    digits = 0b0000001;
                    break;
                case 2:
                    digits = 0b0000011;
                    break;
                case 3:
                    digits = 0b0000111;
                    break;
                case 4:
                    digits = 0b0001111;
                    break;
                case 5:
                    digits = 0b0011111;
                    break;
                case 6:
                    digits = 0b0111111;
                    break;
                case 7:
                    digits = 0b1111111;
                    break;
                default:
                    break;
                }
                hat_led_display_digits(digits);
            }
            else {
                NeoPixel_Display_Solid_Color(0,0,0);
                hat_led_display_digits(0b00000000);
                displayed_neopixel_color = 4;
            }
        // Oled/SSD Updates
        if (display_flag) {
            update_displays(current_unit);
            display_flag = false;
        }

    }
        
}
    