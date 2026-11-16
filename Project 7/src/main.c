/**********************************************************************
 * Name: Project 7
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/07/2026 (mm/dd/yyyy)
 * Creation Date: 10/06/2026 (mm/dd/yyyy)
 * Board: STM32 Nucleo F446ZE
 * Hardware Interface: CMSIS
 * Language: C
 * Description:
    * Mock theramin Using the Ultrasonic Rangefinder, DAC speaker, and 
    * the onboard Neopixels
**********************************************************************/
// C Header Files
#include <stdbool.h>
#include <stdio.h>

// External Header Files
#include <stm32f4xx.h>

// Include Header Files
#include "unit_values.h" //Personal header file with defs for SI prefixes and Decimal Names for numbers ex:HUNDRED = HECTO = 100
#include "logo_bitmap.h" //Bootup Logo Bitmap
#include "speaker_driver.h"
#include "neopixel_driver.h"

// Lib Header Files
#include "cpeg222_driver.h" // Personal header with common pin names for the NANO F446ZE board
#include "oled_driver.h"
#include "uart_driver.h"
#include "ultrasonic_driver.h"


// Defines
#define SECOND (int)(180*MEGA)
#define APB1SECOND (int)(45*MEGA)
#define APB2SECOND (int)(90*MEGA)
#define DELAY 2

//Function Headers
    void set_note();

// Vars
    // Bools
        bool units_mode = false; // false for in, true for cm
        bool thermanin_en = false;
        volatile bool start_sensor_calc = false; // keeps the screen from refreshing when there isnt a calc needed
        bool max_distance = false; // changes screen if their is no object to detect
    // Ints
        volatile int pulse_width = 0; // the mesured pulse width
        int pulse_width_last;
        volatile int time_initial = 0;
        volatile int time_final = 0;
        volatile int current_note_f = 0;
        volatile int current_note = 0;
        volatile uint8_t SysTick_time_tracker = 0;
    // Chars
        volatile char *note_name = "C4";
        char buff[25] = {0};
        char buff2[25] = {0};
        char buff3[25] = {0};
    // uints
        uint8_t digit_selector = 0;
    // floats 
        volatile float object_distance = 0.0f;
// Constants
    const int place[] = {1,10,100,1000};
    const char music_notes[13][3] =  {"C4\0", "C#4\0", "D4\0", "D#4\0", "E4\0" , "F4\0", "F#4\0", "G4\0", "G#4\0", "A4\0", "A#4\0", "B4\0", "OFF\0"};


// Handlers
void EXTI1_IRQHandler(void) {
    if (((ultrasonic_echo_GPIO->IDR & (1 << ultrasonic_echo_pin))  != 0)) {
        time_initial = TIM7->CNT;
    }
    else {
        time_final = (TIM7->CNT);
        pulse_width = time_initial < time_final ? time_final - time_initial : (time_final + 0xFFFF) -  time_initial;
        start_sensor_calc = false;
    }
    EXTI->PR = EXTI_PR_PR1;
}

void TIM5_IRQHandler(void){
    if(TIM5->SR & TIM_SR_UIF){ // Check if the update interrupt flag is set
        // Display the value on the SSD array
        int display_num = (current_note_f/place[digit_selector]) % 10;
        if (display_num == 0 && digit_selector == 3) {
            display_num = 10;
        }
        ssd_display_digit(digit_selector, display_num, (digit_selector == 1 ? 1 : 0));
        digit_selector = (digit_selector + 1) % 4; // Cycle through digitSelect values 0 to 3
        TIM5->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }
}

void SysTick_Handler(void) {
    if (start_sensor_calc == false && SysTick_time_tracker >= 4){
            HCSR04_Trigger();
            start_sensor_calc = true;
            SysTick_time_tracker = 0;
            set_note();
    }
    else {
        SysTick_time_tracker++;
    }
}

// Functions
void set_note() {

    if (pulse_width > (int)(15*148.1f)){
        disable_speaker();
        NeoPixel_SetColor(0,0,0,0);
        NeoPixel_SetColor(1,0,0,0);
        NeoPixel_SetColor(2,0,0,0);
        NeoPixel_SetColor(3,0,0,0);
        current_note_f = 0;
        NeoPixel_Show();
    }
    else if (pulse_width > (int)(14*148.1f)) {
        current_note_f = B4_f;
        current_note = B4;
        NeoPixel_SetColor(0,255,0,0);
        NeoPixel_SetColor(1,255,0,0);
        NeoPixel_SetColor(2,255,0,0);
        NeoPixel_SetColor(3,255,0,0);
        note_name = music_notes[11];
    }
    else if (pulse_width > (int)(13*148.1f)) {
        current_note_f = A4_SH_f;
        current_note = A4_SH;
        NeoPixel_SetColor(0,255,93,0);
        NeoPixel_SetColor(1,255,93,0);
        NeoPixel_SetColor(2,255,93,0);
        NeoPixel_SetColor(3,255,93,0);
        note_name = music_notes[10];
    }
    else if (pulse_width > (int)(12*148.1f)) {
        NeoPixel_SetColor(0,255,185,0);
        NeoPixel_SetColor(1,255,185,0);
        NeoPixel_SetColor(2,255,185,0);
        NeoPixel_SetColor(3,255,185,0);
        current_note_f = A4_f;
        current_note = A4;
        note_name = music_notes[9];
    }
    else if (pulse_width > (int)(11*148.1f)) {
        NeoPixel_SetColor(0,232, 255, 0);
        NeoPixel_SetColor(1,232, 255, 0);
        NeoPixel_SetColor(2,232, 255, 0);
        NeoPixel_SetColor(3,232, 255, 0);
        current_note_f = G4_SH_f;
        current_note = G4_SH;
        note_name = music_notes[8];
    }
    else if (pulse_width > (int)(10*148.1f)) {
        NeoPixel_SetColor(0,139, 255, 0);
        NeoPixel_SetColor(1,139, 255, 0);
        NeoPixel_SetColor(2,139, 255, 0);
        NeoPixel_SetColor(3,139, 255, 0);
        current_note_f = G4_f;
        current_note = G4;
        note_name = music_notes[7];
    }
    else if (pulse_width > (int)(9*148.1f)) {
        NeoPixel_SetColor(0,46, 255, 0);
        NeoPixel_SetColor(1,46, 255, 0);
        NeoPixel_SetColor(2,46, 255, 0);
        NeoPixel_SetColor(3,46, 255, 0);
        current_note_f = F4_SH_f;
        current_note = F4_SH;
        note_name = music_notes[6];
    }
    else if (pulse_width > (int)(8*148.1f)) {
        NeoPixel_SetColor(0,0, 255, 46);
        NeoPixel_SetColor(1,0, 255, 46);
        NeoPixel_SetColor(2,0, 255, 46);
        NeoPixel_SetColor(3,0, 255, 46);
        current_note_f = F4_f;
        current_note = F4;
        note_name = music_notes[5];
    }
    else if (pulse_width > (int)(7*148.1f)) {
        NeoPixel_SetColor(0,0, 255, 139);
        NeoPixel_SetColor(1,0, 255, 139);
        NeoPixel_SetColor(2,0, 255, 139);
        NeoPixel_SetColor(3,0, 255, 139);
        current_note_f = E4_f;
        current_note = E4;
        note_name = music_notes[4];
    }
    else if (pulse_width > (int)(6*148.1f)) {
        NeoPixel_SetColor(0,0, 255, 232);
        NeoPixel_SetColor(1,0, 255, 232);
        NeoPixel_SetColor(2,0, 255, 232);
        NeoPixel_SetColor(3,0, 255, 232);
        current_note_f = D4_SH_f;
        current_note = D4_SH;
        note_name = music_notes[3];
    }
    else if (pulse_width > (int)(5*148.1f)) {
        NeoPixel_SetColor(0,0, 185, 255);
        NeoPixel_SetColor(1,0, 185, 255);
        NeoPixel_SetColor(2,0, 185, 255);
        NeoPixel_SetColor(3,0, 185, 255);
        current_note_f = D4_f;
        current_note = D4;
        note_name = music_notes[2];
    }
    else if (pulse_width > (int)(4*148.1f)) {
        NeoPixel_SetColor(0,0, 93, 255);
        NeoPixel_SetColor(1,0, 93, 255);
        NeoPixel_SetColor(2,0, 93, 255);
        NeoPixel_SetColor(3,0, 93, 255);
        current_note_f = C4_SH_f;
        current_note = C4_SH;
        note_name = music_notes[1];
    }
    else if (pulse_width < (int)(4*148.1f)){
        NeoPixel_SetColor(0,0, 0, 255);
        NeoPixel_SetColor(1,0, 0, 255);
        NeoPixel_SetColor(2,0, 0, 255);
        NeoPixel_SetColor(3,0, 0, 255);
        current_note_f = C4_f;
        current_note = C4;
        note_name = music_notes[0];
    }
    else {
        disable_speaker();
        NeoPixel_SetColor(0,0,0,0);
        NeoPixel_SetColor(1,0,0,0);
        NeoPixel_SetColor(2,0,0,0);
        NeoPixel_SetColor(3,0,0,0);
        current_note_f = 0;
        NeoPixel_Show();
    }
    
}

void refresh_OLED(char *buff_l, char *buff2_l, char *buff3_1) {
    OLED_Print(3,0, "Pulse:             ");
    OLED_Print(3,0, buff_l);
    OLED_Print(4,0, "Distance:          ");
    OLED_Print(4,0, buff2_l);
    OLED_Print(5,0, "Note:              ");
    OLED_Print(5,0, buff3_1);
}

int main() {
    // Hardware Settup
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock
        ENABLE180MHZCLOCK();
        ENABLE_OLED();
        OLED_Render_Screen_Buffer(UD_logo_128x64);
        delay_ms(1000);
        ENABLE_NEOPIXELS();
        ENABLE_SHIELD_SWITCHES();
        ENABLE_ULTRASONIC(90U,2U);
        ENABLE_SYS_TICK(9000000U, 3);
        ENABLE_UART();
        ENABLE_SSD(45,1000,3);
        ENABLE_DAC_SPEAKER(sine_lut, 100, 200);
        delay_ms(2000);
        OLED_Clear();
    // 
    // Screen Settup
        OLED_Print(0,0, "Range Finder Demo");
        OLED_Print(1,0,"B. Trainor 10/23/26");
        snprintf(buff, sizeof(buff), "Pulse: %d us", pulse_width);
        snprintf(buff2, sizeof(buff2), "Distance: %.2f %s", object_distance, (units_mode ? "in":"cm"));
        snprintf(buff3, sizeof(buff3), "Note: OFF");
        refresh_OLED(buff, buff2, buff3);
        NeoPixel_SetColor(0,255,0,0);
        NeoPixel_SetColor(1,255,0,0);
        NeoPixel_SetColor(2,255,0,0);
        NeoPixel_SetColor(3,255,0,0);
        NeoPixel_Show();
    //
    while(1) {
        if (pulse_width_last != pulse_width || read_switch(SW1_H) != units_mode || read_switch(SW2_H) != thermanin_en) {
            units_mode = read_switch(SW1_H); 
            thermanin_en = read_switch(SW2_H);
            pulse_width_last = pulse_width;
            if (pulse_width >= 25000U) {
                max_distance = true;
                }
                else {
                    max_distance = false;
                    if (units_mode) {
                        object_distance = (float)(pulse_width / 58.3f);
                    }
                    else{
                        object_distance = (float)(pulse_width / 148.1f);
                    }
                }
            snprintf(buff, sizeof(buff), "Pulse: %d us", pulse_width);
            if (!max_distance) {
                snprintf(buff2, sizeof(buff2), "Distance: %.2f %s     ", object_distance, (units_mode ? "cm":"in"));
                }
                 else {
                snprintf(buff2, sizeof(buff2), "Distance: OutofRange");
                }
            if (thermanin_en && pulse_width < (int)(15*148.1f)) {
                enable_speaker();
                NeoPixel_Show();
                snprintf(buff3, sizeof(buff3), "Note: %c%c%c", note_name[0],note_name[1],note_name[2]);
                speaker_set_frequency(current_note);
            }
            else{
                disable_speaker();
                speaker_set_frequency(current_note);
                NeoPixel_SetColor(0,0,0,0);
                NeoPixel_SetColor(1,0,0,0);
                NeoPixel_SetColor(2,0,0,0);
                NeoPixel_SetColor(3,0,0,0);
                NeoPixel_Show();
                snprintf(buff3, sizeof(buff3), "Note: OFF");
            }
            refresh_OLED(buff, buff2, buff3);
        }
        serial_printf(buff, sizeof(buff));
        serial_printf(",     ", sizeof(",     "));
        serial_printf(buff2, sizeof(buff2));
        serial_printf(",     ", sizeof(",     "));
        serial_printf(buff3, sizeof(buff3));
        serial_printf("\n", sizeof("\n")); 
    }
        
}
    