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
#include "i2c_driver.h"

uint8_t I2C1_IsDeviceReady(uint8_t addr) {
    // Generate start condition
    I2C1->CR1 |= I2C_CR1_START | I2C_CR1_ACK; // must include ACK for single byte read   
    // Wait for start bit to be sent
    uint32_t timeout = 10000;
    while (!(I2C1->SR1 & I2C_SR1_SB) && --timeout);
    if (timeout == 0) return 0; // Timeout    
    // Send device address
    I2C1->DR = addr << 1; // Write address  
    // Wait for address to be sent
    timeout = 10000;
    while (!(I2C1->SR1 & I2C_SR1_ADDR) && !(I2C1->SR1 & I2C_SR1_AF) && --timeout);  
    if (I2C1->SR1 & I2C_SR1_AF) {
        // NACK received - device not present
        I2C1->SR1 &= ~I2C_SR1_AF; // Clear AF flag
        I2C1->CR1 |= I2C_CR1_STOP;
        return 0;
    }
    if (I2C1->SR1 & I2C_SR1_ADDR) {
        // ACK received - device present
        (void)I2C1->SR1; (void)I2C1->SR2; // Clear ADDR flag
        I2C1->CR1 |= I2C_CR1_STOP;
        return 1;
    }
    // Timeout
    I2C1->CR1 |= I2C_CR1_STOP;
    return 0;
}
void I2C_Start(void) { // Generate a start condition on the I2C bus
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)); // Wait for Start condition generated
}

void I2C_Write(uint8_t data) { // Write a byte of data to the I2C bus
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_TXE)); // Wait for byte transmission buffer empty
}

void I2C_Address(uint8_t addr, uint8_t direction) { // Send the 7-bit address with the specified direction (0 for write, 1 for read)
    I2C1->DR = (addr << 1) | direction;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Wait for address acknowledgment match
    (void)I2C1->SR2; // Clear ADDR flag by reading SR2
}

void I2C_Stop(void) { // Generate a stop condition on the I2C bus
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C_WriteRegister(uint8_t addr, uint8_t reg, uint8_t data) { // Write a single byte of data to a specific register of the I2C device
    I2C_Start();
    I2C_Address(addr, 0); // Write Mode
    I2C_Write(reg);
    I2C_Write(data);
    while (!(I2C1->SR1 & I2C_SR1_BTF)); // Ensure last byte fully shifted out
    I2C_Stop();
}

void I2C_WriteBurstRegister(uint8_t addr, uint8_t reg, const uint8_t *data, uint8_t length) { // Write multiple bytes of data to consecutive registers of the I2C device
    I2C_Start();
    I2C_Address(addr, 0); // Write Mode
    I2C_Write(reg);
    for (uint8_t i = 0; i < length; i++) {
        I2C_Write(data[i]);
    }
    while (!(I2C1->SR1 & I2C_SR1_BTF)); // Ensure final byte transmission completes
    I2C_Stop();
}

uint8_t I2C_ReadRegister(uint8_t addr, uint8_t reg) { // Read a single byte of data from a specific register of the I2C device
    uint8_t data;
    I2C_Start();
    I2C_Address(addr, 0); // Write Mode to select register
    I2C_Write(reg);
    I2C_Start(); // Repeated Start condition
    // For a single-byte read, clear ACK before ADDR is cleared.
    I2C1->CR1 &= ~I2C_CR1_ACK;
    I2C1->DR = (addr << 1) | 1U;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;
    I2C_Stop();
    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    data = I2C1->DR;
    // Restore ACK for subsequent multi-byte transactions.
    I2C1->CR1 |= I2C_CR1_ACK;
    return data;
}

void I2C_ReadBurst(uint8_t addr, uint8_t start_reg, uint8_t *buffer, uint8_t length) { // Read multiple bytes of data from consecutive registers of the I2C device
    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = I2C_ReadRegister(addr, (uint8_t)(start_reg + i));
    }
}