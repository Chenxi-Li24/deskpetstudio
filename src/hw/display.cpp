// src/hw/display.cpp — ST7789V 240x280 -> 135x240 logical canvas
// Verified against Waveshare ESP32-S3-Touch-LCD-1.69 V2.1 HelloWorld example
#include "hw/display.h"
#include <Arduino_GFX_Library.h>

static Arduino_DataBus* s_bus    = nullptr;
static Arduino_ST7789*  s_gfx    = nullptr;
static Arduino_Canvas*  s_canvas = nullptr;

bool hwDisplayInit() {
  // Arduino_HWSPI with explicit SCK/MOSI — critical fix from official example
  s_bus = new Arduino_HWSPI(PIN_LCD_DC, PIN_LCD_CS, PIN_LCD_SCLK, PIN_LCD_MOSI);

  // ST7789(bus, RST, rotation, IPS, w, h, col1, row1, col2, row2)
  s_gfx = new Arduino_ST7789(s_bus, PIN_LCD_RST, 0, true,
                             LCD_PHYS_W, LCD_PHYS_H, 0, 20, 0, 0);

  if (!s_gfx->begin()) {
    Serial.println("hwDisplay: ST7789 gfx->begin() FAILED");
    return false;
  }
  s_gfx->fillScreen(BLACK);

  // Backlight ON — digital per official example (not PWM)
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

  // Canvas: 135x240 logical on 240x280 physical
  s_canvas = new Arduino_Canvas(HW_W, HW_H, s_gfx);
  if (!s_canvas->begin()) {
    Serial.println("hwDisplay: Canvas init failed");
    return false;
  }

  Serial.printf("hwDisplay: ST7789V %dx%d OK, Canvas %dx%d\n",
                LCD_PHYS_W, LCD_PHYS_H, HW_W, HW_H);
  return true;
}

Arduino_Canvas* hwCanvas() { return s_canvas; }

void hwDisplayPush() {
  // Canvas (240x250) on 260-row visible area, shifted down slightly
  static const int OFF_Y = 6;
  uint16_t* src = (uint16_t*)s_canvas->getFramebuffer();
  s_gfx->draw16bitRGBBitmap(0, OFF_Y, src, HW_W, HW_H);
}

void hwDisplayBrightness(uint8_t lvl) {
  if (lvl == 0) digitalWrite(PIN_LCD_BL, LOW);
  else          digitalWrite(PIN_LCD_BL, HIGH);
}

void hwDisplaySleep(bool off) {
  if (off) {
    digitalWrite(PIN_LCD_BL, LOW);
    s_gfx->displayOff();
  } else {
    s_gfx->displayOn();
    digitalWrite(PIN_LCD_BL, HIGH);
  }
}
