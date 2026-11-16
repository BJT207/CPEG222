/************************************************************************
 * Name: BME280 Driver Header
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/5/2026 (mm/dd/yyyy)
 * Creation Date: 11/5/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Additional hardware:
    * CPEG222 Trainer Brd Rev.E
 * Description:
    * Driver for the BME280 over I2C
 * Attribution:
    * Much of the code here is heavily inspired or coppied from demo code
    * found in the CPEG222 Github Page and/or class slides
 * Note:
   * Must Initialize the I2C pins before starting the BME280
************************************************************************/
#ifndef BME280I2C
#define BME280I2C
#include "i2c_driver.h"

   // Device I2C Addresses
   #define BME280_ADDRESS 0x76 // Primary I2C Address
   #define BME280_ADDRESS_ALTERNATE 0x77 // Alternate Address
   // BME280 Register Map
   #define BME280_REG_ID 0xD0
   #define BME280_REG_CTRL_HUM 0xF2
   #define BME280_REG_CTRL_MEAS 0xF4
   #define BME280_REG_CONFIG 0xF5
   #define BME280_REG_PRESS_MSB 0xF7
   #define BME280_REG_CALIB_00 0x88 // Calibration data start
   #define BME280_REG_CALIB_26 0xE1 // More calibration data

   // Structs
   typedef struct {
      uint16_t    Temp0;
      int16_t     Temp1;
      int16_t     Temp2;
      uint16_t    Pressure0;
      int16_t     Pressure1;
      int16_t     Pressure2;
      int16_t     Pressure3;
      int16_t     Pressure4;
      int16_t     Pressure5;
      int16_t     Pressure6;
      int16_t     Pressure7;
      int16_t     Pressure8;
      uint8_t     Humidity0;
      int16_t     Humidity1;
      uint8_t     Humidity2;
      int16_t     Humidity3;
      int16_t     Humidity4;
      int8_t      Humidity5;
   } BME280_CalibData;

   // Global Vars
      BME280_CalibData g_calib_data;
      int32_t g_t_fine; // Fine temperature, needed for Pressure and Humidity compensation
      volatile int32_t g_temperature; // degC * 100
      volatile uint32_t g_pressure; // Pascals
      volatile uint32_t g_humidity; // %RH * 1024
   // Functions
   uint8_t Enable_BME280(void);
   void BME280_ReadCalibrationData(void);
   int32_t BME280_Compensate_Temp(int32_t adc_T);
   uint32_t BME280_Compensate_Pressure(int32_t adc_P);
   uint32_t BME280_Compensate_Humidity(int32_t adc_H);
   void BME280_Read_Data(void);

#endif