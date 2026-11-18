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
#include <stdlib.h>
#include "uart_driver.h"

#define MAX_ADJUSTMENT 20

// Vars
    volatile uint16_t current_pwm[2] = {0};
    volatile uint16_t current_angle[2] = {0};
    volatile uint16_t last_angle[2] = {0};

    volatile int16_t target_rpm[2] = {0};
    volatile int16_t current_rpm[2] = {0};

    volatile uint16_t last_rising_edge[2] = {0};
    volatile uint16_t last_falling_edge[2] = {0};
    
// Headers
    void motor_pwm_calcs(bool is_right);
    void set_motor_speed(bool motor_index, int set_rpm);
    void update_individual_rpm_measurements(bool is_right);
    void update_angle_measurements(bool is_right);
    int16_t calc_ang_delta(bool is_right);

// Exposed Functions
    void update_rpm_measurements(uint delta_time_ms) {
        int16_t delta_angle[2] = {calc_ang_delta(0), calc_ang_delta(1)};
        serial_printf("Angle Delta L %d\n", delta_angle[0]);
        serial_printf("Angle Delta R %d \n", delta_angle[1]);
        current_rpm[0] = ((double)(delta_angle[0] * (60000)) / (3600.0f * delta_time_ms));
        current_rpm[1] = ((double)(delta_angle[1] * (60000)) / (3600.0f * delta_time_ms));
        serial_printf("RPM_READING %d\n",current_rpm[0]);
        serial_printf("RPM_READING %d\n",current_rpm[1]);
    }

    void set_target_rpm(int mode, int desired_rpm) {
        // Sets the target rpm for the individual wheels based on the desired directional rpm and mode
        if (!mode || desired_rpm == 0) {
            set_motor_speed(0, 0); // Left Motor
            set_motor_speed(1, 0); // Right Motor
        }   
        else if (mode == 1) {
            set_motor_speed(0, desired_rpm); // Left Motor
            set_motor_speed(1, -desired_rpm); // Right Motor
        }
        else if (mode == 2) {
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

    void update_pwm( ) {
        motor_pwm_calcs(0); // left
        motor_pwm_calcs(1); // right
    }

    void motor_speed_sync(uint delta_time_ms) {
        update_rpm_measurements(delta_time_ms);
        update_pwm();
    }


// Internal Functions
    int16_t calc_ang_delta(bool is_right) {
        int16_t delta_angle = current_angle[is_right] - last_angle[is_right];
        if (delta_angle > 1800) {
            delta_angle -= 3600; // Wrapped backwards across 0
        } else if (delta_angle < -1800) {
            delta_angle += 3600; // Wrapped forwards across 360
        }
        last_angle[is_right] = current_angle[is_right];
        return delta_angle;
    }

    void motor_pwm_calcs(bool is_right) {

        if (target_rpm[is_right] == 0) { return;}
        
        int16_t adjusted_pwm = current_pwm[is_right];
        if ((int)(target_rpm[is_right]) > (int)(current_rpm[is_right])) {
            adjusted_pwm += 1;
        }
        else {
            adjusted_pwm -= 1;
        }
        if (target_rpm[is_right] > 0) {
            if (adjusted_pwm > 1720) {
                current_pwm[is_right] = 1720;
            }
            else if (adjusted_pwm < 1520) {
                current_pwm[is_right] = 1520;
            }
            else{
                current_pwm[is_right] = adjusted_pwm;
            }
        }
        else {
            if (adjusted_pwm > 1480) {
                current_pwm[is_right] = 1480;
            }
            else if (adjusted_pwm < 1280) {
                current_pwm[is_right] = 1280;
            }
            else{
                current_pwm[is_right] = adjusted_pwm;
            }
        }
        if (is_right) {
            TIM4->CCR3 = (uint32_t)(current_pwm[is_right]);
        }
        else {
            TIM4->CCR1 = (uint32_t)(current_pwm[is_right]);
        }

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
                TIM4->CCR3 = current_pwm[motor_index];
            }
            else {
                TIM4->CCR1 = current_pwm[motor_index];
            }
    }

    void update_angle_measurements(bool is_right) {
        bool is_rising = false;
        if (is_right && (Right_Feed_Back_GPIO->IDR & (0b1 << Right_Feed_Back_Pin))){ 
                is_rising = true;
        }
        else if (Left_Feed_Back_GPIO->IDR & (0b1 << Left_Feed_Back_Pin)){ 
                is_rising = true;
        }
        if (is_rising) {
            uint16_t rising_edge = (is_right ? TIM1->CCR3: TIM1->CCR1);       
            uint16_t high_time = last_falling_edge[is_right] - last_rising_edge[is_right];
            uint16_t cycle_time = rising_edge - last_rising_edge[is_right];
            if (cycle_time > 1000) {
                uint duty_cycle = high_time*1000 /cycle_time;
                current_angle[is_right] = (duty_cycle-29) *3600 /942;            
            }
            last_rising_edge[is_right] = rising_edge;
        }
        else {
            last_falling_edge[is_right] = (is_right ? TIM1->CCR3: TIM1->CCR1);
        }
    }

// Value Fetch Functions
    // TODO: REMOVE AND REPLACE WITH A SINGLE FUNCTION THAT RETURNS THE POINTERS IN A GLOBAL VAR
    uint16_t get_current_pwm(bool is_right) {
        return current_pwm[is_right];
    }
    int16_t get_current_rpm(bool is_right) {
        return current_rpm[is_right];
    }
    uint16_t get_last_angle(bool is_right){
        return last_angle[is_right];
    }
// END TODO