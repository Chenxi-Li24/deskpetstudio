// src/hw/input.cpp — CST816T 触摸
// 旋转编码器 EC11 已完全移除

#include "hw/input.h"
#include "hw/pins.h"
#include <Arduino.h>
#include <Wire.h>
// 测试模式禁用触摸库
#if 0
#include <Adafruit_CST8XX.h>
#endif

static HwTouch s_touch;

// ── CST816T ──
// 测试模式: CST816T 全部禁用
#if 0
static Adafruit_CST8XX s_cst;
static volatile bool   s_tpIrq = false;
static void IRAM_ATTR tpISR() { s_tpIrq = true; }
#endif

bool hwInputInit() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(400000);

  // --- CST816T 触摸 (测试禁用) ---
#if 0
  pinMode(PIN_TP_RST, OUTPUT);
  digitalWrite(PIN_TP_RST, LOW);  delay(10);
  digitalWrite(PIN_TP_RST, HIGH); delay(50);

  if (!s_cst.begin()) {
    Serial.println("hwInput: CST816T begin failed (no touch)");
  } else {
    pinMode(PIN_TP_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TP_INT), tpISR, FALLING);
    Serial.println("hwInput: CST816T OK");
  }
#endif

  Serial.println("hwInput: I2C init OK");
  return true;
}

void hwInputUpdate() {
  // --- 触摸屏 (测试禁用) ---
#if 0
  if (s_tpIrq || s_touch.down) {
    s_tpIrq = false;
    if (s_cst.touched()) {
      s_touch.justPressed  = !s_touch.down;
      s_touch.justReleased = false;
      CST_TS_Point p = s_cst.getPoint();
      s_touch.x = p.x;
      s_touch.y = p.y;

      // Physical touch → Canvas coords: canvas origin at GFX (0, OFF_Y=6) → physical row 26
      s_touch.x = constrain(p.x, 0, HW_W - 1);
      s_touch.y = constrain(p.y - 26, 0, HW_H - 1);
      s_touch.down = true;
    } else {
      s_touch.justReleased = s_touch.down;
      s_touch.down         = false;
      s_touch.justPressed  = false;
    }
  }
#endif
}

const HwTouch& hwTouch() { return s_touch; }
