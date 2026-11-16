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

// Src Header Files
#include "enable_hardware/enable_hardware.h"

// Defines
    #define SSD_Precision 2
//Function Headers
    void motor_pwm_calcs(bool is_right);
// Vars

    // Motor Vars
        volatile uint16_t rise[2] = {0,0};     // CCR1 at the last rising edge: counter value when the signal went high
        volatile uint16_t fall[2] = {0,0};     // CCR1 at the last falling edge: counter value when the signal went low
        volatile uint16_t high[2] = {0,0};     // pulse high time in ticks (= us)
        volatile uint16_t cycle[2] = {0,0};    // time between two rising edges in ticks (= us)
        volatile int32_t duty[2] = {0,0};      // duty cycle in tenths of a percent
        volatile int32_t prev_angle[2] = {0,0};     // shaft angle in tenths of a degree
        volatile int32_t angle[2] = {0,0};     // shaft angle in tenths of a degree
        volatile uint32_t captures[2] = {0,0}; // number of feedback cycles captured so far
    // Bools
        volatile bool update_flag = false;
        bool lights_toggle = true;
        bool audio_toggle = true;
        bool alarm_toggle = false; 
        bool flashflip = false;
        
    // Ints
        volatile uint8_t mode = 0;
        volatile unsigned int time_count = 0;
        volatile uint current_pwm[2] = {0};
        volatile uint current_rpm[2] = {0};
        volatile uint current_target_rpm = 0;
        int target_rpm = 1500;
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
        motor_pwm_calcs(false);
    }
    if (TIM1->SR & TIM_SR_CC3IF) {
        motor_pwm_calcs(true);
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
}

// Functions
void update_motor_speed(int motor_idx, int target_rpm) {
    // Note: Origionally written by gemini on 11/13 at 11:10 am, modified to fit project needs
    
    // Calculate angle difference over the 1-second window (in tenths of a degree)
    int32_t angle_diff = angle[motor_idx] - prev_angle[motor_idx];
    // Handle wrap-around at 360 degrees (3599 tenths of a degree)
    if (angle_diff < -1800) {
        angle_diff += 3600;
    } else if (angle_diff > 1800) {
        angle_diff -= 3600;
    }

    // Convert angle change per second to RPM 
    // (10ths of a degree per second / 3600 tenths of a degree per revolution * 60,000 millieconds per min) = RPM
    current_rpm[motor_idx] = (angle_diff * 100) / 6;
    prev_angle[motor_idx] = angle[motor_idx];

    // Simple Proportional (P) Controller to adjust pulse width
    int error = target_rpm - current_rpm[motor_idx];
    int adjustment = error * 2; // Tune this proportional gain constant as needed

    // Clamp adjustment to prevent wild swings
    if (target_rpm == current_target_rpm) {
        if (adjustment > 20) adjustment = 20;
        if (adjustment < -20) adjustment = -20;
    }
    else {
        current_target_rpm = target_rpm;
    }

    current_pwm[motor_idx] += adjustment;

    // Safety limits for continuous rotation servo pulse widths
    if (current_pwm[motor_idx] > 1700) current_pwm[motor_idx] = 1700;
    if (current_pwm[motor_idx] < 1300) current_pwm[motor_idx] = 1300;
    serial_printf("Current Left PWM: %d\n", current_pwm[0]);
    serial_printf("Current Right PWM: %d\n", current_pwm[0]);
    // Apply to hardware registers
    if (motor_idx == 0) {
        TIM4->CCR1 = current_pwm[0]; // Left Motor
    } else {
        TIM4->CCR3 = current_pwm[1]; // Right Motor
    }
}

void motor_pwm_calcs(bool is_right){
    static uint16_t prevRise[2] = {0};       // time of the most recent rising edge
    static uint8_t haveRise[2] = {0};        // 1 once a rising edge has been seen
    static uint16_t lastCycle[2] = {0};      // working copy of the cycle time, published with the other values

    is_right = (TIM1->SR & TIM_SR_CC1IF ?  false : true);
    uint16_t t = is_right ? TIM1->CCR3 : TIM1->CCR1;        // edge time in ticks, reading it clears the flag
    if (GPIOE->IDR & (1 << (is_right? FB_3:FB_1))) {   // pin is high: this was the RISING edge
        if (haveRise[is_right]) {
            lastCycle[is_right] = t - prevRise[is_right];
        }
        prevRise[is_right] = t;
        haveRise[is_right] = 1;
    } else if (haveRise[is_right]) {          // pin is low: this was the FALLING edge (after a rising edge)
        // HIGH TIME = falling time - rising time, the same wrap-safe subtraction.
        uint16_t h = t - prevRise[is_right];
        if (lastCycle[is_right] > 1000 && lastCycle[is_right] < 1200) {     // skip a bad cycle (nominal is 1099 us)
            // Publish every value of this capture together, so main sees one consistent set:
            rise[is_right] = prevRise[is_right];                            // raw ticks, rising edge of this cycle
            fall[is_right] = t;                                   // raw ticks, falling edge of this cycle
            high[is_right] = h;
            cycle[is_right] = lastCycle[is_right];
            // DUTY CYCLE = high time / cycle time. Multiply by 1000 first so the integer
            // result is in tenths of a percent (730 / 1096 = 0.666 = 666 tenths = 66.6 %).
            duty[is_right] = ((int32_t)high[is_right] * 1000) / cycle[is_right];
            int32_t d = duty[is_right];
            if (d < DUTY_MIN) {
                d = DUTY_MIN;
            } else if (d > DUTY_MAX) {
                d = DUTY_MAX;
            }
            angle[is_right] = ((d - DUTY_MIN) * 3599) / (DUTY_MAX - DUTY_MIN);
            captures[is_right]++;
        }
    }
}

void update_displays() {
    OLED_Print(0,0,"CPEG222 F26 Project 9");
    OLED_Print(1,0,"Brandon Trainor");
    OLED_Print(2,0, "%f", 0.123456789);
    OLED_Print(3,0, "%.6f", 0.123456789);
}

int main() {

    Enable_Hardware();

    while(1) {
        uint32_t pot_val = 0;
        read_Hat_POT(&pot_val);
        if (update_flag) {
            if (mode) {
                target_rpm = (int)(pot_val * 140 / 4095);
            }
            else {
                target_rpm = 0;
            }
            ssd_value = target_rpm;
            if (!mode || target_rpm == 0) {
                TIM4->CCR3 = IDLE_PULSE_US; // Sets the speed to 0 for the right motor
                TIM4->CCR1 = IDLE_PULSE_US; // Sets the speed to 0 for the left motor
            }   
            if (mode == 1) {
                update_motor_speed(0, target_rpm); // Left Motor
                update_motor_speed(1, -target_rpm); // Right Motor
            }
            if (mode == 2) {
                update_motor_speed(0, -target_rpm); // Left Motor
                update_motor_speed(1, target_rpm); // Right Motor
            }
            else if (mode == 3) {
                update_motor_speed(0, target_rpm); // Left Motor
                update_motor_speed(1, target_rpm); // Right Motor
            }
            else if (mode == 4) {
                update_motor_speed(0, -target_rpm); // Left Motor
                update_motor_speed(1, -target_rpm); // Right Motor
            }
            update_displays();
            update_flag = false;
        }
        // serial_printf("L_Caps:%d R_Caps:%d L_Angle:%d R_Angle:%d\n", 
        //       captures[0], captures[1], angle[0], angle[1]);
    }
        
}
    