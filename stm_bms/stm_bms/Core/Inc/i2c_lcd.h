#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f1xx_hal.h"

#define LCD_ADDR        (0x27 << 1)
#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_RS          0x01

void lcd_init(void);
void lcd_setCursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_clear(void);
void lcd_command(uint8_t cmd);
void lcd_write_char(uint8_t data);

#endif
