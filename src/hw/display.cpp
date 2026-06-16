// src/hw/display.cpp — ST7789V 240x280 → 135x240 逻辑画布
// P169H002-CTP / Waveshare ESP32-S3-Touch-LCD-1.69 V2.1

#include "hw/display.h"
#include <Arduino_GFX_Library.h>

static Arduino_DataBus* s_bus    = nullptr;
static Arduino_ST7789*  s_gfx    = nullptr;
static Arduino_Canvas*  s_canvas = nullptr;

static const uint8_t BRIGHT_LUT[5] = { 10, 64, 128, 192, 255 };

bool hwDisplayInit() {
  s_bus = new Arduino_HWSPI(PIN_LCD_DC, PIN_LCD_CS);

#ifdef BOARD_WAVESHARE
  // Waveshare 板: IPS=true, 无需 invert
  s_gfx = new Arduino_ST7789(s_bus, PIN_LCD_RST, 0, true,
                             LCD_PHYS_W, LCD_PHYS_H, 0, 20);
#else
  // P169H002-CTP: normally-black 需反转
  s_gfx = new Arduino_ST7789(s_bus, PIN_LCD_RST, 0, false,
                             LCD_PHYS_W, LCD_PHYS_H, 0, 20);
#endif

  if (!s_gfx->begin()) {
    Serial.println("hwDisplay: ST7789 init failed");
    return false;
  }
#ifndef BOARD_WAVESHARE
  s_gfx->invertDisplay(true);
#endif
  s_gfx->fillScreen(BLACK);

  s_canvas = new Arduino_Canvas(HW_W, HW_H, s_gfx);
  if (!s_canvas->begin()) {
    Serial.println("hwDisplay: Canvas init failed");
    return false;
  }

  // 背光 PWM
  ledcSetup(0, 5000, 8);
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 255);  // 背光最大

  Serial.printf("hwDisplay: ST7789V %dx%d OK\n", LCD_PHYS_W, LCD_PHYS_H);
  return true;
}

Arduino_Canvas* hwCanvas() { return s_canvas; }

void hwDisplayPush() {
  const int OFF_X = (LCD_PHYS_W - HW_W) / 2;   // (240-135)/2 = 52
  const int OFF_Y = (LCD_PHYS_H - HW_H) / 2;   // (280-240)/2 = 20

  uint16_t* src = (uint16_t*)s_canvas->getFramebuffer();
  s_gfx->fillScreen(BLACK);
  s_gfx->draw16bitRGBBitmap(OFF_X, OFF_Y, src, HW_W, HW_H);
}

void hwDisplayBrightness(uint8_t lvl) {
  if (lvl > 4) lvl = 4;
  ledcWrite(0, BRIGHT_LUT[lvl]);
}

void hwDisplaySleep(bool off) {
  if (off) {
    ledcWrite(0, 0);
    s_gfx->displayOff();
  } else {
    s_gfx->displayOn();
    hwDisplayBrightness(2);
  }
}
