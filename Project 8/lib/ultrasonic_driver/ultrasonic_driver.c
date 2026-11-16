/**********************************************************************
 * Name: Ultrasonic Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/22/2026 (mm/dd/yyyy)
 * Creation Date: 10/22/2026 (mm/dd/yyyy)
 * Main Board: STM32 Nucleo F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
    * CPEG222 Robot Base Rev.C
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    *  Used to drive a HC-SR04 Ultrasonic Sensor
 * Attribution:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
**********************************************************************/
#include "cpeg222_driver.h"
#include "ultrasonic_driver.h"

void delay_us(uint32_t us);

void HCSR04_Trigger(void) { // Send a 10us pulse to the TRIG_PIN
    GPIOE->BSRR = (1 << ultrasonic_trigger_pin);         // Set TRIG_PIN high
    delay_us(12);                          // Wait at least 10 microseconds
    GPIOE->BSRR = (1 << (ultrasonic_trigger_pin + 16));  // Set TRIG_PIN low
}

void delay_us(uint32_t us) { // Delay for a specified number of microseconds using TIM7
    uint16_t start = (uint16_t)TIM7->CNT; // Record the start time of the delay
    while ((uint16_t)(TIM7->CNT - start) < us) {// Wait until the specified number of microseconds has elapsed
    }
}