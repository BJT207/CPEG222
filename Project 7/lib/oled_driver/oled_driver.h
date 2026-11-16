/**********************************************************************
 * Name: OLED Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 10/21/2026 (mm/dd/yyyy)
 * Date Created:  10/11/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Additional Hardware:
    * CPEG222 Trainer Brd Rev.E
 * Description:
    * Header file for the OLED Driver. Broken out from main CPEG222
    * because of complexity
**********************************************************************/
#ifndef oled_driver
#define oled_driver
#include "cpeg222_driver.h"
#include <stddef.h>

// Pin Definitions
    #define SPI1_SCLK_PIN    5U  // SPI1 SCK pin is connected to PA5
    #define SPI1_MISO_PIN    6U  // SPI1 MISO pin is connected to PA6
    #define SPI1_MOSI_PIN    7U  // SPI1 MOSI pin is connected to PA7
    #define SPI1_CS_PIN      15U  // SPI1 CS pin is connected to PA15 (PA4 is used for analog output)
    #define SPI1_PORT        GPIOA  // SPI1 GPIO port
    #define OLED_DC_PIN      2U  // OLED DC pin is connected to PG2
    #define OLED_RESET_PIN   3U  // OLED RESET pin is connected to PG3
    #define OLED_PORT        GPIOG  // OLED GPIO port
// oled_driver.c function headers
    void OLED_WriteCmd(uint8_t);                                                //function to Write Commands to the OLED
    void OLED_WriteData(const uint8_t *, size_t);
    void OLED_Clear(void);                                                      //function to Clear the OLED Screen
    void OLED_Clear_Page(int page);                                             //function to Clear a single page of the OLED Screen
    void OLED_Print(uint8_t, uint8_t, const char *);                            //function to print letters to the screen
    void OLED_Render_Screen_Buffer(const uint8_t screenbuffer[8][128]);               //Renders a buffer directly to the screen
    void OLED_Play_Bootup_Animaiton();
    void OLED_Init(void);
// Inline function headers                                                      
    static inline void OLED_WriteCmd_16(uint16_t cmd)  {OLED_WriteCmd((uint8_t)((cmd>>8) & 0xFF));   //inline function sub of WriteCMD for 16 bit commands
                                            OLED_WriteCmd((uint8_t)(cmd & 0xFF));}
    static inline void OLED_Chip_Select()    {SPI1_PORT->ODR &= ~(1U << SPI1_CS_PIN);}      //macro to select OLED (CS low)
    static inline void OLED_Chip_Unselect()  {SPI1_PORT->ODR |= (1U << SPI1_CS_PIN);}       //macro to unselect OLED (CS high)
    static inline void OLED_Command_Mode()   {OLED_PORT->ODR &= ~(1U << OLED_DC_PIN);}      //macro to set DC low (command)
    static inline void OLED_Data_Mode()      {OLED_PORT->ODR |= (1U << OLED_DC_PIN);}       //macro to set DC high (data)
    static inline void OLED_Reset_Low()      {OLED_PORT->ODR &= ~(1U << OLED_RESET_PIN);}   //macro to set RESET low
    static inline void OLED_Reset_High()     {OLED_PORT->ODR |= (1U << OLED_RESET_PIN);}    //macro to set RESET high

// Settup Macros
    static inline void ENABLE_OLED() {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOGEN;/*Enable GPIO A and G clock*/ 
        /*Settup AF output pins*/ 
        SPI1_PORT->MODER &= ~((0x3U << (SPI1_SCLK_PIN * 2U)) | 
                            (0x3U << (SPI1_MISO_PIN * 2U)) | 
                            (0x3U << (SPI1_MOSI_PIN * 2U)) | 
                            (0x3U << (SPI1_CS_PIN * 2U))); /*Clear mode bits for SPI1 pins and CS pin*/ 
        SPI1_PORT->MODER |= (ALT_FUNC_MODE << (SPI1_SCLK_PIN * 2U))  |
                            (ALT_FUNC_MODE << (SPI1_MISO_PIN * 2U)) |
                            (ALT_FUNC_MODE << (SPI1_MOSI_PIN * 2U)) |
                            (OUTPUT_MODE << (SPI1_CS_PIN * 2U)); /*Set SPI1 at AF and CS pin as output*/
        /*Set the alternate function pins to SPI*/
        SPI1_PORT->AFR[0] &= ~((0xFU << (SPI1_SCLK_PIN * 4U)) |
                            (0xFU << (SPI1_MISO_PIN * 4U)) |
                            (0xFU << (SPI1_MOSI_PIN * 4U))); /*Clear alternate function bits for SPI1 pins*/
        SPI1_PORT->AFR[0] |= (5U << (SPI1_SCLK_PIN * 4U)) | 
                            (5U << (SPI1_MISO_PIN * 4U)) |
                            (5U << (SPI1_MOSI_PIN * 4U)); /*Set alternate function 5 for SPI1 pins*/ 
        /*Set SPI pin to run at full 180MHz clock speed*/ 
        SPI1_PORT->OSPEEDR |= (0x3U << (SPI1_SCLK_PIN * 2U)) | 
                            (0x3U << (SPI1_MOSI_PIN * 2U)); /*Set high speed for SPI1 SCK and MOSI pins*/ 
        /*Settup normal output pins*/ 
        OLED_PORT->MODER &= ~((0x3U << (OLED_DC_PIN * 2U)) | 
                            (0x3U << (OLED_RESET_PIN * 2U)));/*Clear mode bits for OLED DC and RESET pins*/ 
        OLED_PORT->MODER |= (OUTPUT_MODE << (OLED_DC_PIN * 2U)) | 
                            (OUTPUT_MODE << (OLED_RESET_PIN * 2U)); /*Set OLED DC and RESET pins as output*/ 
        /*
            Startup for the OLED Display
            Sets the pins in order to high which turns on the display
        */ 
        OLED_Chip_Unselect(); /*Turns on display and tells it to ignore data during startup*/ 
        OLED_Data_Mode(); /*Sets the data mode to display data*/
        OLED_Reset_High(); /*Sets the Reset mode to high to high (therefore disabling the reset)*/ 
        /*
        Settup the SPI clock
        */ 
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; /*Enable SPI1 clock*/ 
        SPI1->CR1 = SPI_CR1_MSTR | /*Set the SPI mode to "Master"*/ 
                    SPI_CR1_SSM  | /*disables the hardware chip select pin*/
                    SPI_CR1_SSI  | /*If not set to high, chip thinks another master disabled master mode and defaults back to slave mode*/ 
                    SPI_CR1_BR_2 | 
                    SPI_CR1_BR_1; /*SPI_CR1_BR_1, SPI_CR1_BR_2 are used in combo to set */ 
        SPI1->CR2 = 0; /*Insures all the settings in cr2 are disabled*/ 
        SPI1->CR1 |= SPI_CR1_SPE; /*Enable SPI1*/ 
    
        OLED_Reset_Low();
        delay_ms(10);
        OLED_Reset_High();
        delay_ms(10);
        
        OLED_WriteCmd(0xAE);        /*Display off*/ 
        OLED_WriteCmd_16(0xD580);   /*Set display clock divide ratio/oscillator frequency*/ 
        OLED_WriteCmd_16(0xA83F);   /*Set multiplex ratio*/ 
        OLED_WriteCmd_16(0xD300);   /*Set display offset*/ 
        OLED_WriteCmd(0x40);        /*Set start line address*/ 
        OLED_WriteCmd_16(0xAD88);   /*Set DC-DC control mode*/ 
        OLED_WriteCmd(0xA1);        /*Set segment re-map*/ 
        OLED_WriteCmd(0xC8);        /*Set COM output scan direction*/ 
        OLED_WriteCmd_16(0xDA12);   /*Set COM pins hardware configuration*/ 
        OLED_WriteCmd_16(0x817F);   /*Set contrast control*/ 
        OLED_WriteCmd_16(0xD922);   /*Set pre-charge period*/ 
        OLED_WriteCmd_16(0xDB20);   /*Set VCOMH deselect level*/ 
        OLED_WriteCmd(0xA4);        /*Entire display ON*/ 
        OLED_WriteCmd(0xA6);        /*Set normal display mode*/ 
        OLED_WriteCmd(0xAF);        /*Display ON*/ 
        OLED_Clear(); 
    };

#endif