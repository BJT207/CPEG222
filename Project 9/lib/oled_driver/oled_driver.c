/**********************************************************************
 * Name: OLED Driver 
 * Author: Brandon Trainor
 * Email: bjtrainor2007@gmail.com
 * Last Modified: 11/13/2026 (mm/dd/yyyy)
 * Date Created:  10/12/2026 (mm/dd/yyyy)
 * Main Board: STM32 NUCLEO F446ZE
 * Additional Hardware:
    * CPEG222 Trainer Brd Rev.E
 * Description:
    * Driver for the OLED screen on the CPEG222 Trainer Brd 
 * Attribution:
    * Much of the code here is coppied directly from demo code
    * found in the CPEG222 Github Page
**********************************************************************/
#include "stm32f446xx.h"
#include "cpeg222_driver.h"
#include "oled_driver.h"
#include "personal_logo_bitmap.h"
#include <stddef.h>
#include <stdarg.h>

#include "uart_driver.h"


// //Static Constants
   static const uint8_t Font5x7[95][5] = {
      {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00}, {0x14,0x7F,0x14,0x7F,0x14},
      {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62}, {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00},
      {0x00,0x1C,0x22,0x41,0x00}, {0x00,0x41,0x22,0x1C,0x00}, {0x14,0x08,0x3E,0x08,0x14}, {0x08,0x08,0x3E,0x08,0x08},
      {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00}, {0x20,0x10,0x08,0x04,0x02},
      {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00}, {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
      {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
      {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00}, {0x00,0x56,0x36,0x00,0x00},
      {0x08,0x14,0x22,0x41,0x00}, {0x14,0x14,0x14,0x14,0x14}, {0x00,0x41,0x22,0x14,0x08}, {0x02,0x01,0x51,0x09,0x06},
      {0x32,0x49,0x79,0x41,0x3E}, {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22},
      {0x7F,0x41,0x41,0x22,0x1C}, {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01}, {0x3E,0x41,0x49,0x49,0x7A},
      {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01}, {0x7F,0x08,0x14,0x22,0x41},
      {0x7F,0x40,0x40,0x40,0x40}, {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E},
      {0x7F,0x09,0x09,0x09,0x06}, {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46}, {0x46,0x49,0x49,0x49,0x31},
      {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F}, {0x3F,0x40,0x38,0x40,0x3F},
      {0x63,0x14,0x08,0x14,0x63}, {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}, {0x00,0x7F,0x41,0x41,0x00},
      {0x02,0x04,0x08,0x10,0x20}, {0x00,0x41,0x41,0x7F,0x00}, {0x04,0x02,0x01,0x02,0x04}, {0x40,0x40,0x40,0x40,0x40},
      {0x00,0x01,0x02,0x04,0x00}, {0x20,0x54,0x54,0x54,0x78}, {0x7F,0x48,0x44,0x44,0x38}, {0x38,0x44,0x44,0x44,0x20},
      {0x38,0x44,0x44,0x48,0x7F}, {0x38,0x54,0x54,0x54,0x18}, {0x08,0x7E,0x09,0x01,0x02}, {0x0C,0x52,0x52,0x52,0x3E},
      {0x7F,0x08,0x04,0x04,0x78}, {0x00,0x44,0x7D,0x40,0x00}, {0x20,0x40,0x44,0x3D,0x00}, {0x7F,0x10,0x28,0x44,0x00},
      {0x00,0x41,0x7F,0x40,0x00}, {0x7C,0x04,0x18,0x04,0x78}, {0x7C,0x08,0x04,0x04,0x78}, {0x38,0x44,0x44,0x44,0x38},
      {0x7C,0x14,0x14,0x14,0x08}, {0x08,0x14,0x14,0x18,0x7C}, {0x7C,0x08,0x04,0x04,0x08}, {0x48,0x54,0x54,0x54,0x20},
      {0x04,0x3F,0x44,0x40,0x20}, {0x3C,0x40,0x40,0x20,0x7C}, {0x1C,0x20,0x40,0x20,0x1C}, {0x3C,0x40,0x30,0x40,0x3C},
      {0x44,0x28,0x10,0x28,0x44}, {0x0C,0x50,0x50,0x50,0x3C}, {0x44,0x64,0x54,0x4C,0x44}, {0x00,0x08,0x36,0x41,0x00},
      {0x00,0x00,0x7F,0x00,0x00}, {0x00,0x41,0x36,0x08,0x00}, {0x08,0x04,0x08,0x10,0x08}
   };

// // Static Functions
static inline void SPI_WriteByte(uint8_t data){ //macro to write a byte via SPI
    while (!(SPI1->SR & SPI_SR_TXE));// Wait until TX buffer is empty
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_TXE));// Wait until TX buffer is empty again
    while (SPI1->SR & SPI_SR_BSY);// Wait until SPI is not busy
}

static void OLED_SetPos(uint8_t page, uint8_t col) {// Set the cursor position on the OLED display
    uint8_t x = (uint8_t)(col + 2U);// Calculate the actual column position with an offset of 2
    OLED_WriteCmd((uint8_t)(0xB0U | (page & 0x0FU)));// Set the page address
    OLED_WriteCmd((uint8_t)(0x00U | (x & 0x0FU)));// Set the lower nibble of the column address
    OLED_WriteCmd((uint8_t)(0x10U | ((x >> 4) & 0x0FU)));// Set the upper nibble of the column address
}

static void OLED_PutChar(uint8_t page, uint8_t col, char c) {// Write a single character to the display at the specified page and column
    uint8_t idx;
    uint8_t space = 0x00;

    if ((c < 32) || (c > 126)) {// Replace non-printable characters with '?'
        c = '?';
    }
    idx = (uint8_t)(c - 32);// Calculate the index into the font array
    OLED_SetPos(page, col);// Set the cursor to the specified page and column
    OLED_WriteData(Font5x7[idx], 5);// Write the character data to the display
    OLED_WriteData(&space, 1);// Write a space column after the character
}

static void OLED_Writebuffer(int page_start, int page_stop, int column_start, int column_stop, const uint8_t screenbuffer[8][128]) {
    // SH1106 RAM is 132 columns wide; Physical Col 0 matches internal index 2
    uint8_t physical_col_offset = 2; 
    for (uint8_t page = page_start; page < page_stop; page++) {
        // 1. Issue command to increment active target Page pointer (0xB0 -> 0xB7)
        OLED_WriteCmd(0xB0 + page); 
        // 2. Clear out column vectors to the starting point
        OLED_WriteCmd(0x00 + (physical_col_offset & 0x0F));        // Lower 4-bits
        OLED_WriteCmd(0x10 + ((physical_col_offset >> 4) & 0x0F)); // Higher 4-bits
        // 3. Fast sequence stream all 128 data columns over the bus
        for (uint16_t col = column_start; col < column_stop; col++) {
            OLED_WriteData(&screenbuffer[page][col], 1);
        }
    }
}

// // Functions
void OLED_WriteCmd(uint8_t cmd){ //macro to write a command to the OLED
    OLED_Chip_Select();// Select the OLED (CS low)
    OLED_Command_Mode();// Set DC low for command
    SPI_WriteByte(cmd);// Write the command byte via SPI
    OLED_Chip_Unselect();// Unselect the OLED (CS high)
}

void OLED_WriteData(const uint8_t *buf, size_t len){ //macro to write data to the OLED
   OLED_Chip_Select();// Select the OLED (CS low)
   OLED_Data_Mode();// Set DC high for data
   size_t i;
   for (i = 0; i < len; ++i) {// Loop through each byte in the buffer
      SPI_WriteByte(buf[i]);// Write each byte via SPI
   }
   OLED_Chip_Unselect();// Unselect the OLED (CS high)
}

void OLED_Clear(void) {// Clear the display by writing zeros to all pages
    for (uint8_t page = 0; page < 8U; ++page) {// Iterate through all 8 pages
        OLED_Clear_Page(page);
    }
}
void OLED_Clear_Page(int page) {// Clear the display by writing zeros to all pages
    OLED_SetPos(page, 0);// Set the cursor to the beginning of the current page
      OLED_Chip_Select();// Select the OLED (CS low)
      OLED_Data_Mode();// Set DC high for data
      for (uint16_t i = 0U; i < 128U; ++i) {// Loop through each byte in the buffer
        SPI_WriteByte(0b0U);// Write each byte via SPI
      }
      OLED_Chip_Unselect();// Unselect the OLED (CS high)
}

void OLED_Print(uint8_t page, uint8_t col,  char *s, ...) {// Print a string to the display starting at the specified page and column
    va_list args;
    va_start(args, s);
    if (s == NULL) {
        return;
    }
    for (char *p = s; *p != '\0'; p++) {
        if (*p == '%' && *(p + 1) != '\0') {
            p++; // Move to the specifier character
            switch (*p) {
                case 'd': { // Integer
                    int i = va_arg(args, int);
                    char int_str[12] = "";
                    snprintf(int_str, sizeof(int_str), "%d", i);
                    for (char *int_ptr = int_str; *int_ptr != '\0'; int_ptr++){
                        OLED_PutChar(page, col, *int_ptr);
                        col = (uint8_t)(col + 6U);// Move the column position for the next character
                        if (col > 122U) {// Stop if the column exceeds the display width
                            goto end_of_column;
                        }
                    }
                    break;
                }
                case 's': { // String
                    char *string = va_arg(args, char *);
                    for (char *str_pntr = string; *str_pntr != '\0'; str_pntr++) {
                        OLED_PutChar(page, col, *str_pntr);
                        col = (uint8_t)(col + 6U);// Move the column position for the next character
                        if (col > 122U) {// Stop if the column exceeds the display width
                            goto end_of_column;
                        }
                    }
                    break;
                }
                case '.': { // Float (promoted to double in va_arg)
                    int places = 0;
                    int i = 1;
                    double f = va_arg(args, double);
                    for (;; i++){
                        if (*(p + i) >= '0' && *(p + i) <= '9') {
                            places = (places * 10) + (*(p + i) - '0');
                        }
                        else if (*(p+i) == 'f') {
                            char float_str[64] = {0};

                            snprintf(float_str, sizeof(float_str), "%.*f", places, f);
                            for (int j = 0; float_str[j] != '\0' && j < 64; j++ ) {
                                OLED_PutChar(page, col, float_str[j]);
                                col = (uint8_t)(col + 6U);// Move the column position for the next character
                                if (col > 122U) {// Stop if the column exceeds the display width
                                    goto end_of_column;
                        }
                            }
                            p = p + i;
                            break;
                        }
                        else {
                            // print everything between
                        }
                    }
                    break;
                }
                 case 'f': {// Float (promoted to double in va_arg)
                    char float_str[64] = {0};
                    double f = va_arg(args, double);
                    snprintf(float_str, sizeof(float_str), "%f", f);
                    for (int j = 0; float_str[j] != '\0' && j < 64; j++ ) {
                        OLED_PutChar(page, col, float_str[j]);
                        col = (uint8_t)(col + 6U);// Move the column position for the next character
                        if (col > 122U) {// Stop if the column exceeds the display width
                            goto end_of_column;
                        }
                    }
                    break;
                }
                default:
                    putchar('%');
                    putchar(*p);
            }
        }
        else if (*p == '\0') {
            break;
        }
        else {
            OLED_PutChar(page, col,*p);
            col = (uint8_t)(col + 6U);// Move the column position for the next character
            if (col > 122U) {// Stop if the column exceeds the display width
                break;
            }
        }
    }
    end_of_column:;
}

void OLED_Render_Screen_Buffer(const uint8_t screenbuffer[8][128]) {
    OLED_Writebuffer(0,8,0,128, screenbuffer);
}

void OLED_Play_Bootup_Animaiton() {
    OLED_Writebuffer(0,4,0,22, personal_logo);
    
    delay_ms(1000);
    OLED_Writebuffer(0,4,0,34, personal_logo);
    delay_ms(100);
    OLED_Writebuffer(0,4,0,46, personal_logo);
    delay_ms(100);
    OLED_Writebuffer(0,4,0,57, personal_logo);
    delay_ms(100);
    OLED_Writebuffer(0,8,0,39, personal_logo);
};
