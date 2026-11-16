/**********************************************************************
 * Name: Project 6
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/07/2026 (mm/dd/yyyy)
 * Creation Date: 10/06/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Rangefinder made using the HCSR04 Untrasonic Sensor
**********************************************************************/
// C Header Files
#include <stdbool.h>
#include <stdio.h>

// External Header Files
#include <stm32f4xx.h>

// Include Header Files
#include "unit_values.h" //Personal header file with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
#include "logo_bitmap.h"

// Lib Header Files
#include "cpeg222_driver.h" // Personal header with common pin names for the NANO F446ZE board
#include "oled_driver.h"
#include "uart_driver.h"
#include "ultrasonic_driver.h"

// Defines
#define SECOND (int)(180*MEGA)
#define APB1SECOND (int)(45*MEGA)
#define APB2SECOND (int)(90*MEGA)
#define DELAY 2
//Notes


// Vars
    // Bools
        bool units_mode = false; // false for in, true for cm
        volatile bool start_sensor_calc = false; // keeps the screen from refreshing when there isnt a calc needed
        bool max_distance = false; // changes screen if their is no object to detect
    // Ints
        volatile int pulse_width = 0; // the mesured pulse width
        volatile float object_distance = 0.0f;
        int pulse_width_last;
        volatile int time_initial = 0;
        volatile int time_final = 0;
    // Chars
        char buff[25] = {0};
        char buff2[25] = {0};

// Constants

void EXTI1_IRQHandler(void) {
    if (((ultrasonic_echo_GPIO->IDR & (1 << ultrasonic_echo_pin))  != 0)) {
        time_initial = TIM7->CNT;
    }
    else {
        time_final = (TIM7->CNT);
        pulse_width = time_initial < time_final ? time_final - time_initial : (time_final + 0xFFFF) -  time_initial;
        start_sensor_calc = false;
    }
    EXTI->PR = EXTI_PR_PR1;
}

void refresh_OLED(char *buff_l, char *buff2_l) {
    OLED_Print(3,0, "Pulse:             ");
    OLED_Print(3,0, buff_l);
    OLED_Print(4,0, "Distance:          ");
    OLED_Print(4,0, buff2_l);
}

int main() {
    // Hardware Settup
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
        ENABLE180MHZCLOCK();
        ENABLE_SHIELD_SWITCHES();
        ENABLE_ULTRASONIC(90U,2U);
        ENABLE_UART();
        ENABLE_OLED();
        OLED_Render_Screen_Buffer(UD_logo_128x64);
        delay_ms(2000);
        OLED_Clear();
    // 
    // Screen Settup
        OLED_Print(0,0, "Range Finder Demo");
        OLED_Print(1,0,"B. Trainor 10/23/26");
        snprintf(buff, sizeof(buff), "Pulse: %d us", pulse_width);
        snprintf(buff2, sizeof(buff2), "Distance: %.2f %s", object_distance, (units_mode ? "in":"cm"));
        refresh_OLED(buff, buff2);
    //
    while(1) {
        if (start_sensor_calc == false){
            HCSR04_Trigger();
            start_sensor_calc = true;
        }
        delay_ms(100);
        if (pulse_width_last != pulse_width || read_switch(SW1_H) != units_mode) {
            units_mode = read_switch(SW1_H); 
            pulse_width_last = pulse_width;
            if (pulse_width >= 25000U) {
                max_distance = true;
            }
            else {
                max_distance = false;
                if (units_mode) {
                    object_distance = (float)(pulse_width / 58.3f);
                }
                else{
                    object_distance = (float)(pulse_width / 148.1f);
                }
            }
            snprintf(buff, sizeof(buff), "Pulse: %d us", pulse_width);
            if (!max_distance) {
                snprintf(buff2, sizeof(buff2), "Distance: %.2f %s     ", object_distance, (units_mode ? "cm":"in"));
            }
            else {
                snprintf(buff2, sizeof(buff2), "Distance: OutofRange");
            }
            refresh_OLED(buff, buff2);
        }
        serial_printf(buff, sizeof(buff));
        serial_printf(",     ", sizeof(",     "));
        serial_printf(buff2, sizeof(buff2));
        serial_printf("\n", sizeof("\n")); 
    }
        
}
    