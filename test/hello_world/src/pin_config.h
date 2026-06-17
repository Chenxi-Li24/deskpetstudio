#pragma once

// Waveshare ESP32-S3-Touch-LCD-1.69 V2.1

// SPI Display (ST7789V2, 4-wire)
#define LCD_DC     7
#define LCD_CS     8
#define LCD_SCK    13
#define LCD_MOSI   15
#define LCD_RST    38
#define LCD_BL     14

// Resolution
#define LCD_WIDTH   240
#define LCD_HEIGHT  280

// I2C
#define I2C_SDA    5
#define I2C_SCL    4

// Touch (CST816T)
#define TP_RST     10
#define TP_INT     11
