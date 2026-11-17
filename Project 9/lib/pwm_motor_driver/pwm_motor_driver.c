/**********************************************************************
 * Name: PWM Motor Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/12/2026 (mm/dd/yyyy)
 * Creation Date: 11/12/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Used to enable and drive the motors
**********************************************************************/
#include "stm32f446xx.h"

#include "pwm_motor_driver.h"



// Vars
    volatile uint current_pwm[2] = {0};
    volatile uint last_angle[2] = {0};

    volatile int target_rpm[2] = {0};
    volatile int current_rpm[2] = {0};

    volatile uint16_t last_rising_edge[2] = {0};
    volatile uint16_t last_falling_edge[2] = {0};
    
// Headers
    void motor_pwm_calcs(bool);
    void set_motor_speed(bool, int);
    void update_measurements(bool);

void motor_pwm_calcs(bool is_right) {

}
void calculate_rpm(bool is_right) {
   
}
void set_motor_speed(bool motor_index, int set_rpm) {
        target_rpm[motor_index] = set_rpm;
        if (target_rpm[motor_index] < -140) {
            target_rpm[motor_index] = -140;
        }
        if (target_rpm[motor_index] > 140) {
            target_rpm[motor_index] = 140;
        }
        if (target_rpm[motor_index] > 0) {
            current_pwm[motor_index] = 1520 + (int)(200 * target_rpm[motor_index] / 140);
        }
        else if (target_rpm[motor_index] < 0) {
            current_pwm[motor_index] = 1480 + (int)(200 * target_rpm[motor_index] / 140);
        }
        else {
            current_pwm[motor_index] = IDLE_PULSE_US;
        }
        if (current_pwm[motor_index] > 1720) {current_pwm[motor_index] = 1720;}
        else if (current_pwm[motor_index] < 1280) {current_pwm[motor_index] = 1280;}
        if (motor_index) {
            TIM4->CCR1 = current_pwm[motor_index];
        }
        else {
            TIM4->CCR3 = current_pwm[motor_index];
        }
}

void update_rpm_measurements(bool is_right) {
    if (is_right) {
        if (Right_Feed_Back_GPIO->IDR & (0b1 << Right_Feed_Back_Pin)){ 
            uint16_t rising_edge = TIM1->CCR3;
            
            uint16_t high_time = last_falling_edge[1] - last_rising_edge[1];
            uint16_t cycle_time = rising_edge - last_rising_edge[1];
            if (cycle_time > 1000) {
                uint duty_cycle = high_time*1000 /cycle_time;
                uint angle = (duty_cycle-29) *3600 /942;
                current_rpm[1] = ((int)(angle - last_angle[1]) * 60000) / (RPM_WINDOW_MS * 3600);
                last_angle[1] = angle;
            }
            last_rising_edge[1] = rising_edge;
        }
        else {
            last_falling_edge[1] = TIM1->CCR3;
        }
    }
    else{
        if (Left_Feed_Back_GPIO->IDR & (0b1 << Left_Feed_Back_Pin)){ 
            uint16_t rising_edge = TIM1->CCR1;
            uint16_t high_time = last_falling_edge[0] - last_rising_edge[0];
            uint16_t cycle_time = rising_edge - last_rising_edge[0];
            if (cycle_time > 1000) {
                uint duty_cycle = high_time * 1000/cycle_time;
                uint angle = (duty_cycle-29) *3600 /942;
                current_rpm[0] = ((int)(angle - last_angle[0]) * 60000) / (RPM_WINDOW_MS * 3600);
                last_angle[0] = angle;
            }
            last_rising_edge[0] = rising_edge;
        }
        else {
            last_falling_edge[0] = TIM1->CCR1;
        }
    }
}

void update_speed(int mode, int desired_rpm) {
    if (!mode || desired_rpm == 0) {
        set_motor_speed(0, 0); // Left Motor
        set_motor_speed(1, 0); // Right Motor
    }   
    if (mode == 1) {
        set_motor_speed(0, desired_rpm); // Left Motor
        set_motor_speed(1, -desired_rpm); // Right Motor
    }
    if (mode == 2) {
        set_motor_speed(0, -desired_rpm); // Left Motor
        set_motor_speed(1, desired_rpm); // Right Motor
    }
    else if (mode == 3) {
        set_motor_speed(0, desired_rpm); // Left Motor
        set_motor_speed(1, desired_rpm); // Right Motor
    }
    else if (mode == 4) {
        set_motor_speed(0, -desired_rpm); // Left Motor
        set_motor_speed(1, -desired_rpm); // Right Motor
    }
}

uint get_current_pwm(bool is_right) {
    return current_pwm[is_right];
}
uint get_current_rpm(bool is_right) {
    return current_rpm[is_right];
}
uint get_last_angle(bool is_right){
    return last_angle[is_right];
}