/**********************************************************************
 * Name: Line Follower Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/19/2026 (mm/dd/yyyy)
 * Creation Date: 11/19/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Used to drive the line follower IR sensor
**********************************************************************/
#include <stdint.h>
#include "line_follower_driver.h"
volatile uint16_t adc3_buffer[5] = {0};

