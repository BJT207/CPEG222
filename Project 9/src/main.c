/**********************************************************************
 * Name: Project 9
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/07/2026 (mm/dd/yyyy)
 * Creation Date: 10/06/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Servo Movement Demo Project
**********************************************************************/
// C Header Files
#include <stdbool.h>
#include <stdio.h>

// External Header Files
#include <stm32f4xx.h>

// Include Header Files
#include "unit_values.h" //Personal header file with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
// #include "logo_bitmap.h" //Bootup Logo Bitmap

// Lib Header Files
#include "cpeg222_driver.h" // Personal header with common pin names for the NANO F446ZE board
#include "oled_driver.h"
#include "bme280_driver.h"
#include "speaker_driver.h"
#include "neopixel_driver.h"
#include "pwm_motor_driver.h"
#include "uart_driver.h"
#include "pwm_motor_driver.h"

// Src Header Files
#include "enable_hardware/enable_hardware.h"

// Defines
    #define SSD_Precision 2
    #define uint unsigned int

//Function Headers

// Vars
    // Bools
        volatile bool update_flag = false;
        bool lights_toggle = true;
        bool audio_toggle = true;
        bool alarm_toggle = false; 
        bool flashflip = false;
        
    // Ints
        volatile uint8_t mode = 0;
        volatile uint time_count = 0;
        volatile uint8_t rpm_setting = 0;
        volatile uint tickcount = 0;
    // Chars
    
    // uints
        volatile uint16_t ssd_value;
                 uint8_t  displayed_neopixel_color=4;
    // floats 
        
// Constants
// Handlers

void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        mode = 0;
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_LEFT_H)) {
        if (mode == 0 || mode == 3) {
            mode = 3;
        }
        else {
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        if (mode == 0 || mode == 4) {
            mode = 4;
        }
        else {
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_UP_H)) {
        if (mode == 0 || mode == 1) {
            mode = 1;
        }
        else {
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    if(EXTI->PR & (1 << BTN_DOWN_H)) {
        if (mode == 0 || mode == 2) {
            mode = 2;
        }
        else {
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_DOWN_H); // Clear Interupt bit
    }   
}

void TIM1_CC_IRQHandler(void) {
    if (TIM1->SR & TIM_SR_CC1IF) {      // an edge (rising OR falling) was captured on CH1
        update_angle_measurements(0);
        TIM1->SR = ~TIM_SR_CC1IF;
    }
    if (TIM1->SR & TIM_SR_CC3IF) {
        update_angle_measurements(1);
        TIM1->SR = ~TIM_SR_CC3IF;
    }
}

void TIM5_IRQHandler(void){
    if(TIM5->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
        ssd_interupt_controller(ssd_value, 5);
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }
}

void SysTick_Handler(void) {
    if (time_count >= 100 && !update_flag) {
        update_flag = true;
        time_count = 0;
    }
    else if (!update_flag) {
        time_count++;
    };
    tickcount++;
}

// Functions

void update_displays() {
    OLED_Print(0,0,"CPEG222 F26 Project 9");
    OLED_Print(1,0,"Brandon Trainor");
    OLED_Print(2,0, "L RPM %d PWM %d", get_current_rpm(0), (int)(get_current_pwm(0)));
    OLED_Print(3,0, "R RPM %d PWM %d", get_current_rpm(1), (int)(get_current_pwm(1)));
}



int main() {

    Enable_Hardware();

    while(1) {
        uint32_t pot_val = 0;
        read_Hat_POT(&pot_val);
        if (update_flag) {
            uint8_t potential_rpm = (uint8_t)((((float)(pot_val) / 4095.0f)) * 140);
            if (potential_rpm != rpm_setting && mode){
                rpm_setting = potential_rpm;
                set_target_rpm(mode, rpm_setting);
                ssd_value = rpm_setting;
            }
            else if (!mode){
                rpm_setting = 0;
                ssd_value = 0;
                set_target_rpm(0, 0);
            }
            motor_speed_sync(tickcount);
            tickcount = 0;
            serial_printf("\n");
            serial_printf("RPM Val: %d\n", rpm_setting);
            serial_printf("Mode Val: %d\n", mode);
            serial_printf("Last Angle L: %f\n", (float)(get_last_angle(0)* 0.1f));
            serial_printf("Last Angle R: %f\n", (float)(get_last_angle(1)* 0.1f));
            serial_printf("RPM L: %d\n", get_current_rpm(0));
            serial_printf("RPM R: %d\n", get_current_rpm(1));
            update_displays();
            update_flag = false;
        }
    }
        
}
    