#include "i2c_lcd.h"

extern I2C_HandleTypeDef hi2c1;

static void expanderWrite(uint8_t data) {
	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &data, 1, HAL_MAX_DELAY);
}

static void pulseEnable(uint8_t data) {
    expanderWrite(data | LCD_ENABLE);
    HAL_Delay(1);
    expanderWrite(data & ~LCD_ENABLE);
    HAL_Delay(1);
}

static void write4bits(uint8_t data) {
    expanderWrite(data | LCD_BACKLIGHT);
    pulseEnable(data | LCD_BACKLIGHT);
}

static void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t high = value & 0xF0;
    uint8_t low  = (value << 4) & 0xF0;
    write4bits(high | mode);
    write4bits(low  | mode);
}

void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0x00);   // RS = 0: commande
}

void lcd_write_char(uint8_t data) {
    lcd_send(data, LCD_RS); // RS = 1: donnée
}

void lcd_init() {
    HAL_Delay(50);

    // Séquence d'initialisation en mode 4 bits
    write4bits(0x30); HAL_Delay(5);
    write4bits(0x30); HAL_Delay(1);
    write4bits(0x30); HAL_Delay(1);
    write4bits(0x20); HAL_Delay(1); // Passe en 4 bits

    lcd_command(0x28); // 2 lignes, font 5x8
    lcd_command(0x0C); // Écran ON, curseur OFF
    lcd_command(0x06); // Auto-incrément curseur
    lcd_command(0x01); // Clear display
    HAL_Delay(2);
}

void lcd_setCursor(uint8_t row, uint8_t col) {
    uint8_t addr = col + (row == 0 ? 0x00 : 0x40);
    lcd_command(0x80 | addr);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_write_char((uint8_t)*str++);
    }
}

void lcd_clear() {
    lcd_command(0x01);
    HAL_Delay(2);
}
