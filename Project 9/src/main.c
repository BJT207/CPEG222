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
        volatile unsigned int mode = 0;
        volatile uint time_count = 0;
        volatile uint8_t rpm_setting = 0;
        volatile uint tickcount = 0;
    // Chars
    
    // uints
        volatile uint16_t ssd_value;
                 uint8_t  displayed_neopixel_color=4;
    // floats 
    
    // Structs
        motor_values_strct *motor_values = NULL;
// Constants
// Handlers

void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        mode = 0;
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_LEFT_H)) {
        if (mode == 0) {
            mode = 3;
        }
        else if (mode != 3) {
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        if (mode == 0) {
            mode = 4;
        }
        else  if (mode != 4){
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_UP_H)) {
        if (mode == 0 || mode == 1) {
            mode = 1;
        }
        else {
            mode = 0;
        }
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_DOWN_H)) {
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
    bool right = false;
    uint32_t right_time;
    bool edge_right;
    bool skipped_right = false;
    bool left = false;
    uint32_t left_time;
    bool edge_left;
    bool skipped_left = false;
    if (TIM1->SR & TIM_SR_CC1OF ) {
        TIM1->SR &= ~TIM_SR_CC1OF;
        skipped_right = true;
    }
    if (TIM1->SR & TIM_SR_CC3OF ) {
        TIM1->SR &= ~TIM_SR_CC3OF;
        skipped_left = true;
    }
    if (TIM1->SR & TIM_SR_CC1IF) {      // an edge (rising OR falling) was captured on CH1
        right = true;
        right_time = TIM1->CCR1;
        edge_right = (bool)(Right_Feed_Back_GPIO->IDR & (0b1 << Right_Feed_Back_Pin));
    }
    if (TIM1->SR & TIM_SR_CC3IF) {
        left = true;
        left_time = TIM1->CCR3;
        edge_left = (bool)(Left_Feed_Back_GPIO->IDR & (0b1 << Left_Feed_Back_Pin));
    }
    if (right == true) {
        update_angle_measurements(1, edge_right, right_time, skipped_right);
    }
    if (left == true) {
        update_angle_measurements(0, edge_left, left_time, skipped_left);

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
    OLED_Print(2,0, "L Angle %d", *(motor_values->left_angle));
    OLED_Print(3,0, "R Angle %d", *(motor_values->right_angle));
}



int main() {

    Enable_Hardware();
    motor_values = get_motor_ponters();
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
            update_displays();
            update_flag = false;
            // serial_printf("Left Angle %d\n", *(motor_values->left_angle));
            // serial_printf("Right Angle %d\n", *(motor_values->right_angle));
        }
        
    }
        
}
    