/**********************************************************************
 * Name: Project 10
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/19/2026 (mm/dd/yyyy)
 * Creation Date: 11/19/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Line Follower Demo
**********************************************************************/
// C Header Files
#include <stdbool.h>
#include <stdio.h>

// External Header Files
#include <stm32f4xx.h>

// Lib Header Files
#include "cpeg222_driver.h" // Personal header with common pin names for the NANO F446ZE board
#include "oled_driver.h"
#include "uart_driver.h"
#include "pwm_motor_driver.h"
#include "line_follower_driver.h"

// Src Header Files
#include "enable_hardware/enable_hardware.h"

// Defines
    #define uint unsigned int

//Function Headers

// Vars
    // Bools
        volatile bool update_flag = false;        
    // Ints
        volatile unsigned int mode = 0;
        volatile bool mode_flag = true;
        volatile uint time_count = 0;
        volatile uint8_t rpm_setting = 0;
        volatile uint tickcount = 0;
    // Chars
        static const char * mode_text[5] = {
            "Stopped", "Forward", "Backwards", "Left", "Right"
        };
    // uints
        volatile uint16_t ssd_value;
    // floats 
    
    // Structs
        motor_values_strct *motor_values = NULL;
// Constants
// Handlers

void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR & (1 << BTN_CENTER_H)) {
        
        EXTI->PR |= (1 << BTN_CENTER_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_LEFT_H)) {
        
        EXTI->PR |= (1 << BTN_LEFT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_RIGHT_H)) {
        
        EXTI->PR |= (1 << BTN_RIGHT_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_UP_H)) {
        
        EXTI->PR |= (1 << BTN_UP_H); // Clear Interupt bit
    }
    else if(EXTI->PR & (1 << BTN_DOWN_H)) {
        
        EXTI->PR |= (1 << BTN_DOWN_H); // Clear Interupt bit
    }   
}

void TIM1_CC_IRQHandler(void) {
    Motor_Interupt_Controller(); // takes care of the angle calculations for the motors
}

void TIM5_IRQHandler(void){
    // Drives the SSD
    if(TIM5->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
        ssd_interupt_controller(ssd_value, 5);
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }
}

void SysTick_Handler(void) {
    if (time_count >= 100 && !update_flag) { //triggers an update every 100 ms in main
        update_flag = true;
        time_count = 0;
    }
    else if (!update_flag) {
        time_count++;
    };

    tickcount++; // used for rpm measurements
}

// Functions

void update_displays() {
    OLED_Print(0,0,"CPEG222 F26 Project 9");
    OLED_Print(1,0,"Brandon Trainor");
    OLED_Print(2,0, "11/12 Fall 2026");
    OLED_Print(4,0, "L RPM: %d PWM:%d", *(motor_values->current_rpm_left), *(motor_values->current_pwm_left));
}

int main() {

    Enable_Hardware();

    motor_values = get_motor_ponters();

    while(1) {
        uint32_t pot_val = 0;
        read_Hat_POT(&pot_val);
            uint8_t potential_rpm = (uint8_t)((((float)(pot_val) / 4095.0f)) * 140);    
        if (update_flag) {
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
            update_displays();
            update_flag = false;
            mode_flag = true;
        }
        
    }
        
}



    