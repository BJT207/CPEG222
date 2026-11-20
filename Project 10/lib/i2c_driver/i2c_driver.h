/************************************************************************
 * Name: I2C Driver
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/5/2026 (mm/dd/yyyy)
 * Creation Date: 11/5/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Description:
    * Driver for the PB8 and PB9 I2C Pins
 * Attribution:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
************************************************************************/
#ifndef I2C_DRIVER
#define I2C_DRIVER
#include "cpeg222_driver.h"
#include "stm32f446xx.h"
    
    // Functions
    uint8_t I2C1_IsDeviceReady(uint8_t addr);
    uint8_t I2C_ReadRegister(uint8_t addr, uint8_t reg);
    void I2C_Start(void);
    void I2C_Write(uint8_t data);
    void I2C_Address(uint8_t addr, uint8_t direction);
    void I2C_Stop(void);
    void I2C_WriteRegister(uint8_t addr, uint8_t reg, uint8_t data);
    void I2C_WriteBurstRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t length);
    void I2C_ReadBurst(uint8_t addr, uint8_t start_reg, uint8_t *buffer, uint8_t length);

    // Inline Functions
    static inline void Enable_I2C() {
        //Clock
        RCC->AHB1ENR |= I2C_PINS_GPIOEN; // Enable GPIOB
        RCC->APB1ENR |= I2C_ENABLE;
        // Pins
        I2C_PINS_GPIO->MODER &= ~((0x3 << (I2C_SDA * 2)) | (0x3 << (I2C_SCL * 2))); // Clear the Mode register
        I2C_PINS_GPIO->MODER |= (ALT_FUNC_MODE << (I2C_SDA * 2)) | (ALT_FUNC_MODE << (I2C_SCL * 2)); // Set GPIOB to Alternate Function
        // AF mode
        I2C_PINS_GPIO->AFR[1] &= ~((0b1111 << ((I2C_SDA - 8)*4)) | (0b1111 << ((I2C_SCL - 8)*4))); //Clear AF for the SDA and SCL pins
        I2C_PINS_GPIO->AFR[1] |= (0b0100 << ((I2C_SDA-8)*4)) | (0b0100 << ((I2C_SCL-8)*4)); // Set the alternate function to 4 for SDA/SCL
        // Set the pin to Open drain to allow for pullup resistor
        I2C_PINS_GPIO->OTYPER |= (1 << I2C_SDA) | (1 << I2C_SCL); // Open-drain to allow for pullup resistor
        I2C_PINS_GPIO->OSPEEDR |= (0b10 << (I2C_SDA*2)) | (0b10 << (I2C_SCL*2)); // Settup High Speed mode for I2C as described in the datasheet
        // I2C1 software reset to ensure everything is reset to default
        I2C1->CR1 |= I2C_CR1_SWRST; // Set SWRST bit
        for (volatile int i = 0; i < 1000; i++); // Short delay
        I2C1->CR1 &= ~I2C_CR1_SWRST; // Clear SWRST bit
        for (volatile int i = 0; i < 1000; i++); // Short delay
        // Configure I2C
        I2C1->CR1 = 0; // Ensure that I2C is off
        I2C1->CR2 |= (45U); // Sets the input clock to 45MHz
        I2C1->CCR |= (45U); // Sets the SCL Clock to 100kHz 
        I2C1->CCR = 225 ; // 100kHz, calculated by (FINAL_CLK/PBCLK)/2 [where FINAL_CLK=100kHz and PBCLK=45MHz]
        I2C1->TRISE = 46; // Maximum rise time setting
        I2C1->CR1 |= I2C_CR1_PE;
    }

#endif