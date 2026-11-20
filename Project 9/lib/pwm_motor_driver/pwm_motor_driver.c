/**********************************************************************
 * Name: PWM Motor Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/17/2026 (mm/dd/yyyy)
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
    // User Setting Vars
        volatile int16_t target_rpm[2] = {0,0};
    // Measured Angle
        volatile bool has_rise[2] = {false,false};
        volatile uint16_t current_angle[2] = {0,0};
        volatile uint16_t last_angle[2] = {0,0};
    // Angle Calculation
        volatile uint16_t prior_rising_edge[2] = {0,0};
        volatile uint16_t cycle_duration[2] = {0,0};
    // RPM Calculation
        volatile int16_t current_rpm[2] = {0,0};
    // PWM 
        volatile uint16_t current_pwm[2] = {0,0};
    
// Headers
    void motor_pwm_calcs(bool is_right);
    void set_motor_speed(bool motor_index, int set_rpm);
    void update_individual_rpm_measurements(bool is_right);
    int16_t calc_ang_delta(bool is_right);

// Exposed Functions
    void update_rpm_measurements(uint delta_time_ms) {
        int16_t delta_angle[2] = {calc_ang_delta(0), calc_ang_delta(1)};
        current_rpm[0] = ((double)(delta_angle[0] * (60000)) / (3600.0f * 100));
        current_rpm[1] = ((double)(delta_angle[1] * (60000)) / (3600.0f * 100));
        // serial_printf("RPM_READING %d\n",current_rpm[0]);
        // serial_printf("RPM_READING %d\n",current_rpm[1]);
    }

    void set_target_rpm(int mode, int desired_rpm) {
        // Sets the target rpm for the individual wheels based on the desired directional rpm and mode
        if (!mode || desired_rpm == 0) {
            set_motor_speed(0, 0); // Left Motor
            set_motor_speed(1, 0); // Right Motor
        }   
        else if (mode == 1) { // Forward
            set_motor_speed(0, -desired_rpm); // Left Motor
            set_motor_speed(1, desired_rpm); // Right Motor
        }
        else if (mode == 2) { // Backwards
            set_motor_speed(0, desired_rpm); // Left Motor
            set_motor_speed(1, -desired_rpm); // Right Motor
        }
        else if (mode == 3) { // Left
            set_motor_speed(0, desired_rpm); // Left Motor
            set_motor_speed(1, 0); // Right Motor
        }
        else if (mode == 4) { // Right
            set_motor_speed(0, 0); // Left Motor
            set_motor_speed(1, -desired_rpm); // Right Motor
        }
        else if (mode == 5) { // Tank Turn Left
            set_motor_speed(0, desired_rpm); // Left Motor
            set_motor_speed(1, desired_rpm); // Right Motor
        }
        else if (mode == 6) { // Tank Turn Right
            set_motor_speed(0, -desired_rpm); // Left Motor
            set_motor_speed(1, -desired_rpm); // Right Motor
        }
    }

    void update_pwm() {
        motor_pwm_calcs(0); // left
        motor_pwm_calcs(1); // right
    }

    void motor_speed_sync(uint delta_time_ms) {
        update_rpm_measurements(delta_time_ms);
        update_pwm();
    }


// Internal Functions
    int16_t calc_ang_delta(bool is_right) {

        int16_t delta_angle =  current_angle[is_right] - last_angle[is_right];
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
        
        int16_t adjusted_pwm = (int16_t)current_pwm[is_right];
        int16_t pwm_adjustment = (current_rpm[is_right] - target_rpm[is_right]);
            if (pwm_adjustment < -MAX_ADJUSTMENT){
                pwm_adjustment = -MAX_ADJUSTMENT;
            }
            if (pwm_adjustment > MAX_ADJUSTMENT) {
                pwm_adjustment = MAX_ADJUSTMENT;
        }
        adjusted_pwm += pwm_adjustment;
        if (target_rpm[is_right] < 0) {            
            if (adjusted_pwm > 1720) {
                current_pwm[is_right] = 1720;
            }
            else if (adjusted_pwm < 1500) {
                current_pwm[is_right] = 1500;
            }
            else{
                current_pwm[is_right] = adjusted_pwm;
            }
        }
        else {
            if (adjusted_pwm > 1500) {
                current_pwm[is_right] = 1500;
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

    void update_angle_measurements(bool is_right, bool is_rising, uint16_t edge_time, bool reset_has_rise) {
        if (reset_has_rise) {
            has_rise[is_right] = 0;
        }
        if (is_rising) {
            if (has_rise[is_right]) {
                cycle_duration[is_right] = edge_time - prior_rising_edge[is_right];
            }
            else {
                has_rise[is_right] = true;
            }
            prior_rising_edge[is_right] = edge_time;
        }
        else if (has_rise[is_right]) {
            uint16_t high_time = edge_time - prior_rising_edge[is_right];
            
            if ((cycle_duration[is_right] > 1000) && (cycle_duration[is_right] < 1200)) {
                uint duty_cycle = ((int32_t)high_time*1000) / cycle_duration[is_right];
                if (duty_cycle < DUTY_MIN) {
                    duty_cycle = DUTY_MIN;
                }
                else if (duty_cycle > DUTY_MAX) {
                    duty_cycle = DUTY_MAX;
                }
                current_angle[is_right] = (uint16_t)(duty_cycle-DUTY_MIN) *3599 / (DUTY_MAX - DUTY_MIN);            
            }
        }
    }

// Value Fetch Functions
    motor_values_strct * get_motor_ponters() {
        motor_values_strct *motor_values = (motor_values_strct*)malloc(sizeof(motor_values_strct));
        motor_values->left_angle = &current_angle[0];
        motor_values->right_angle = &current_angle[1];
        motor_values->current_rpm_left = &current_rpm[0];
        motor_values->current_rpm_right = &current_rpm[1];
        motor_values->current_pwm_left = &current_pwm[0];
        motor_values->current_pwm_right = &current_pwm[1];
        return motor_values;
    }