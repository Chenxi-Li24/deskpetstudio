// src/hw/input.h — CST816T 触摸

#pragma once
#include <stdint.h>

struct HwTouch {
  bool    down;
  int16_t x, y;
  bool    justPressed, justReleased;
};

bool          hwInputInit();
void          hwInputUpdate();
const HwTouch& hwTouch();    // 触摸屏
