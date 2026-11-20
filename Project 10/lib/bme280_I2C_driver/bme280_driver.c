/************************************************************************
 * Name: BME280 Driver
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
************************************************************************/
#include <stdint.h>
#include "bme280_driver.h"
#include "i2c_driver.h"

// Functions
   uint8_t Enable_BME280(void) {
      uint8_t id;
      id = I2C_ReadRegister(BME280_ADDRESS, BME280_REG_ID);
      if (id != 0x60) {
         return 0; // Chip ID mismatch
      }
      BME280_ReadCalibrationData();
      I2C_WriteRegister(BME280_ADDRESS, BME280_REG_CTRL_HUM, 0x01); // Set Humidity oversampling x1
      I2C_WriteRegister(BME280_ADDRESS, BME280_REG_CONFIG, (0x05 << 5)); // Set Config (standby 1000ms, filter off)
      // Set Ctrl_Meas (Temp x1, Press x1, Normal mode)
      I2C_WriteRegister(BME280_ADDRESS, BME280_REG_CTRL_MEAS, (0x01 << 5) | (0x01 << 2) | 0x03);
      return 1; // Success
   }

   void BME280_ReadCalibrationData(void) {
      uint8_t calib[26];
      I2C_ReadBurst(BME280_ADDRESS, BME280_REG_CALIB_00, calib, 26);
      g_calib_data.Temp0 = (calib[1] << 8) | calib[0];
      g_calib_data.Temp1 = (calib[3] << 8) | calib[2];
      g_calib_data.Temp2 = (calib[5] << 8) | calib[4];
      g_calib_data.Pressure0 = (calib[7] << 8) | calib[6];
      g_calib_data.Pressure1 = (calib[9] << 8) | calib[8];
      g_calib_data.Pressure2 = (calib[11] << 8) | calib[10];
      g_calib_data.Pressure3 = (calib[13] << 8) | calib[12];
      g_calib_data.Pressure4 = (calib[15] << 8) | calib[14];
      g_calib_data.Pressure5 = (calib[17] << 8) | calib[16];
      g_calib_data.Pressure6 = (calib[19] << 8) | calib[18];
      g_calib_data.Pressure7 = (calib[21] << 8) | calib[20];
      g_calib_data.Pressure8 = (calib[23] << 8) | calib[22];
      g_calib_data.Humidity0 = calib[25];
      //Stop because the two halves of the calibration data aren't next to eachother in the register layout
      uint8_t calib_h[7];
      I2C_ReadBurst(BME280_ADDRESS, BME280_REG_CALIB_26, calib_h, 7);
      g_calib_data.Humidity1 = (calib_h[1] << 8) | calib_h[0];
      g_calib_data.Humidity2 = calib_h[2];
      g_calib_data.Humidity3 = (calib_h[3] << 4) | (calib_h[4] & 0x0F);
      g_calib_data.Humidity4 = (calib_h[5] << 4) | (calib_h[4] >> 4);
      g_calib_data.Humidity5 = (int8_t)calib_h[6];
   }

   // BME280 Compensation Formulas (from Bosch datasheet)
   // Returns temperature in DegC, resolution 0.01 DegC. Output value of “5123” equals 51.23 DegC.
   int32_t BME280_Compensate_Temp(int32_t adc_T) {
      int32_t var1, var2, T;
      var1 = ((((adc_T >> 3) - ((int32_t)g_calib_data.Temp0 << 1))) * ((int32_t)g_calib_data.Temp1)) >> 11;
      var2 = (((((adc_T >> 4) - ((int32_t)g_calib_data.Temp0)) * ((adc_T >> 4) - ((int32_t)g_calib_data.Temp1))) >> 12) *

      ((int32_t)g_calib_data.Temp2)) >> 14;

      g_t_fine = var1 + var2;
      T = (g_t_fine * 5 + 128) >> 8;
      return T;
   }

   // Returns pressure in Pa as an unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
   uint32_t BME280_Compensate_Pressure(int32_t adc_P) {
      int64_t var1, var2, p;
      var1 = ((int64_t)g_t_fine) - 128000;
      var2 = var1 * var1 * (int64_t)g_calib_data.Pressure5;
      var2 = var2 + ((var1 * (int64_t)g_calib_data.Pressure4) << 17);
      var2 = var2 + (((int64_t)g_calib_data.Pressure3) << 35);
      var1 = ((var1 * var1 * (int64_t)g_calib_data.Pressure2) >> 8) + ((var1 * (int64_t)g_calib_data.Pressure1) << 12);
      var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)g_calib_data.Pressure0) >> 33;
      if (var1 == 0) {
      return 0; // avoid exception caused by division by zero
      }
      p = 1048576 - adc_P;
      p = (((p << 31) - var2) * 3125) / var1;
      var1 = (((int64_t)g_calib_data.Pressure8) * (p >> 13) * (p >> 13)) >> 25;
      var2 = (((int64_t)g_calib_data.Pressure7) * p) >> 19;
      p = ((p + var1 + var2) >> 8) + (((int64_t)g_calib_data.Pressure6) << 4);
      // Bosch reference integer algorithm yields pressure in Q24.8 format (Pa * 256).
      // Convert to Pascals by shifting right by 8 so callers get true Pa units.
      return (uint32_t)(p >> 8);
   }

   // Returns humidity in %RH as an unsigned 32 bit integer. Output value of “47445” represents 47445/1024 = 46.333 %RH
   uint32_t BME280_Compensate_Humidity(int32_t adc_H) {
      int32_t v_x1_u32r;
      v_x1_u32r = (g_t_fine - ((int32_t)76800));
      v_x1_u32r = (((((adc_H << 14) - (((int32_t)g_calib_data.Humidity3) << 20) - (((int32_t)g_calib_data.Humidity4) * v_x1_u32r)) +
      ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)g_calib_data.Humidity5)) >> 10) *
      (((v_x1_u32r * ((int32_t)g_calib_data.Humidity2)) >> 11) + ((int32_t)32768))) >> 10) +
      ((int32_t)2097152)) * ((int32_t)g_calib_data.Humidity1) + 8192) >> 14));
      v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)g_calib_data.Humidity0)) >> 4));
      v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
      v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
      return (uint32_t)(v_x1_u32r >> 12);
   }

   void BME280_Read_Data() {
      uint8_t data[8];
      I2C_ReadBurst(BME280_ADDRESS, BME280_REG_PRESS_MSB, data, 8);
      int32_t adc_P = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
      int32_t adc_T = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
      int32_t adc_H = (data[6] << 8) | data[7];
      g_temperature = BME280_Compensate_Temp(adc_T);
      g_pressure = BME280_Compensate_Pressure(adc_P);
      g_humidity = BME280_Compensate_Humidity(adc_H);
   }