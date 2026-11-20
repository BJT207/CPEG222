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
#ifndef PWM_MOTOR_DRIVER
#define PWM_MOTOR_DRIVER
    #include "stm32f446xx.h"
    #include "cpeg222_driver.h"
    #ifndef uint
    #define uint unsigned int
    #endif

    typedef struct 
    {
        volatile uint16_t *right_angle;
        volatile uint16_t *left_angle;
        volatile int16_t  *current_rpm_left;
        volatile int16_t  *current_rpm_right;
        volatile uint16_t *current_pwm_left;
        volatile uint16_t *current_pwm_right;
    }motor_values_strct;
    
    // Pin Settings
        #define Left_Servo_Pin          PWM1
        #define Right_Servo_Pin         PWM3
        #define Left_Servo_GPIO         PWM1_GPIO
        #define Right_Servo_GPIO        PWM3_GPIO
        #define Left_Feed_Back_Pin      FB_1
        #define Right_Feed_Back_Pin     FB_3
        #define Left_Feed_Back_GPIO     FB_3_GPIO
        #define Right_Feed_Back_GPIO    FB_1_GPIO
    // Servo Settings
        #define IDLE_PULSE_US   1500    // stop pulse (inside the 1480 to 1520 us dead band)

        #define DUTY_MIN        29      // 2.9 percent in tenths of a percent = 0 degrees
        #define DUTY_MAX        971     // 97.1 percent in tenths of a percent = 360 degrees
        #define RPM_WINDOW_MS   100     // time between angle readings
    // pwm_motor_driver.c funciton headers
        void set_target_rpm(int mode, int desired_rpm);
        void update_angle_measurements(bool is_right, bool is_rising, uint16_t edge_time, bool reset_has_rise);
        void update_rpm_measurements(uint delta_time_ms);
        void update_pwm();
        void motor_speed_sync(uint time_delta_ms);

        motor_values_strct * get_motor_ponters();
    // inline funcitons
    static inline void ENABLE_MOTORS() {
        // Enable Pin and Timer Clocks
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
            RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
        // Settup PWM for the Motor Pins
            Left_Servo_GPIO->MODER &= ~(0b11 << (Left_Servo_Pin*2)); // clear the pin modes just in case
            Right_Servo_GPIO->MODER &= ~(0b11 << (Right_Servo_Pin*2)); // clear the pin modes just in case
            Left_Servo_GPIO->MODER |= ALT_FUNC_MODE << (Left_Servo_Pin*2); // set the pins to alternate function
            Right_Servo_GPIO->MODER |= ALT_FUNC_MODE << (Right_Servo_Pin*2); // set the pins to alternate function
            #if (Left_Servo_Pin > 7)
                GPIOD->AFR[1] &= ~(0xF << ((Left_Servo_Pin - 8) * 4));
                GPIOD->AFR[1] |= 0x2 << ((Left_Servo_Pin - 8) * 4);
            #else
                GPIOD->AFR[0] &= ~(0xF << (Left_Servo_Pin * 4));
                GPIOD->AFR[0] |= 0x2 << (Left_Servo_Pin * 4);
            #endif
            #if (Right_Servo_Pin > 7)
                GPIOD->AFR[1] &= ~(0xF << ((Right_Servo_Pin - 8) * 4));
                GPIOD->AFR[1] |= 0x2 << ((Right_Servo_Pin - 8) * 4);
            #else
                GPIOD->AFR[0] &= ~(0xF << (Right_Servo_Pin * 4));
                GPIOD->AFR[0] |= 0x2 << (Right_Servo_Pin * 4);
            #endif
        // Timer 4 (PWM Timer) Setup
            TIM4->PSC = 89U; // At a clockspeed of 90MHz, 90/(89+1) = 1us
            TIM4->ARR = 19999; // sets the clock to fire every 20ms = 50 Hz
            TIM4->CCR3 = IDLE_PULSE_US; // Sets the speed to 0 for the right motor
            TIM4->CCR1 = IDLE_PULSE_US; // Sets the speed to 0 for the left motor
            TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
            TIM4->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);   // pwm mode 1
            TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_OC3M);
            TIM4->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);   // pwm mode 1
            TIM4->CCER |= TIM_CCER_CC1E | TIM_CCER_CC3E;
            TIM4->EGR = TIM_EGR_UG;
            TIM4->CR1 |= TIM_CR1_CEN;


        // Timer 1 (Input capture) Settup
            // 1. clocks
            RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
            RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
            // 2. pe9 and pe13 to alternate function, af1 = tim1
            Left_Feed_Back_GPIO->MODER &= ~(0x3 << (Left_Feed_Back_Pin * 2));
            Left_Feed_Back_GPIO->MODER |=  (0x2 << (Left_Feed_Back_Pin * 2));
            #if (Left_Feed_Back_Pin > 7)
            Left_Feed_Back_GPIO->AFR[1] &= ~(0xF << ((Left_Feed_Back_Pin - 8) * 4));
            Left_Feed_Back_GPIO->AFR[1] |=  (0x1 << ((Left_Feed_Back_Pin - 8) * 4));
            #else
            Left_Feed_Back_GPIO->AFR[0] &= ~(0xF << (Left_Feed_Back_Pin * 4));
            Left_Feed_Back_GPIO->AFR[0] |=  0x1 << (Left_Feed_Back_Pin * 4);
            #endif
            Right_Feed_Back_GPIO->MODER &= ~(0x3 << (Right_Feed_Back_Pin * 2));
            Right_Feed_Back_GPIO->MODER |=  (0x2 << (Right_Feed_Back_Pin * 2));
            #if (Right_Feed_Back_Pin > 7)
            Right_Feed_Back_GPIO->AFR[1] &= ~(0xF << ((Right_Feed_Back_Pin - 8) * 4));
            Right_Feed_Back_GPIO->AFR[1] |=  (0x1 << ((Right_Feed_Back_Pin - 8) * 4));
            #else
            Right_Feed_Back_GPIO->AFR[0] &= ~(0xF << (Right_Feed_Back_Pin * 4));
            Right_Feed_Back_GPIO->AFR[0] |=  0x1 << (Right_Feed_Back_Pin * 4);
            #endif
            // 3. free running timer
            TIM1->PSC = 179;                            // 180 MHz / 180 = 1 us per tick
            TIM1->ARR = 0xFFFF;                         // wraps every 65.5 ms
            // 4. ch1 captures both edges of pe9 and pe13 (one channel per pin)
            TIM1->CCMR1 &= ~(TIM_CCMR1_CC1S);
            TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;            // cc1s = 01, ch1 input is ti1
            TIM1->CCMR2 &= ~(TIM_CCMR2_CC3S);
            TIM1->CCMR2 |= TIM_CCMR2_CC3S_0;
            TIM1->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP
                        | TIM_CCER_CC3P | TIM_CCER_CC3NP; // set up to capture on rising and falling edges for both pins
            TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC3E;                // enable capture
            // 5. interrupt on every captured edge
            TIM1->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC3IE;
            NVIC_SetPriority(TIM1_CC_IRQn, 1);
            NVIC_EnableIRQ(TIM1_CC_IRQn);
            // 6. go
            TIM1->EGR = TIM_EGR_UG;
            TIM1->SR = 0;                               // clear flags from setup
            TIM1->CR1 |= TIM_CR1_CEN;
    }
#endif