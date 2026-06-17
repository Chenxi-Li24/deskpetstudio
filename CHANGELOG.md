# CHANGELOG

## [Unreleased] — 2026-06-17

### Added

#### Display
- **Full-screen status page `drawStatus()`** — 240×250 Canvas fills 1.69" ST7789V visible area
  - Title bar + Clawd state badge (color-coded)
  - WiFi SSID (green), IP address, RSSI right-aligned
  - Clawd state large centered with per-state color
  - Stats: Level, Tokens, Approvals, Denials, Velocity
  - System: Heap, Uptime (two-column)
  - Battery: % + mV + USB indicator + visual bar (green/yellow/red)
  - Flash usage, pet name + deskpet.local footer
  - Graceful non-WiFi states

#### Permission System
- `POST /api/permission` — create request with conflict detection (409)
- `GET /api/permission?id=xxx` — poll result (pending/approved/denied)
- `permDraw()` — on-screen dialog: title bar, tool, hint (36-char wrap), Approve/Deny buttons
- `permInput()` — touch hit-testing on button rects
- `permTick()` — 60s auto-timeout
- Screen takeover during active permission, returns to status after

#### BLE Provisioning
- `provision_wifi.py` — bleak-based WiFi credential write over BLE

#### Test Code
- `test/waveshare_s3/` — Waveshare official LVGL demo (WiFi manager + permission reference)
- `test/hello_world/` — minimal display test

### Changed

#### Pin Fix (Waveshare ESP32-S3-Touch-LCD-1.69 V2.1)
- LCD SPI: GPIO13→6, 15→7, 8→5, 7→4, 38→8, 14→15 (all corrected to official pin_config.h)
- I2C: SDA 5→11, SCL 4→10
- Touch: RST 10→13, INT 11→14

#### Display Driver
- `Arduino_HWSPI` constructor now passes SCK/MOSI explicitly
- Backlight simplified from PWM to digital on/off
- Canvas expanded 135×240 → 240×250, push offset changed from centered to top-aligned +6px

#### Input System
- **EC11 rotary encoder completely removed**: ISRs, ADC button read, rotation delta
- `hwInputUpdate()` simplified to touch-only polling
- `HwBtn`/`HwEnc` structs and all accessors deleted
- API `/api/status`: `enc1` → `touch` (down/x/y)

#### Main Loop
- Added `hwDisplayInit()` + boot splash screen
- 500ms timer renders status or permission dialog
- Added includes: `<LittleFS.h>`, `"hw/power.h"`, `"stats.h"`
- Removed encoder debug serial prints

### Known Issues
- CST816T touch driver disabled with `#if 0` (pending debug)
- Status screen hand-drawn with Arduino_GFX, no LVGL framework
- Backlight only on/off, no multi-level brightness

### V2 Roadmap
- [ ] Enable CST816T touch (remove `#if 0`)
- [ ] QMI8658 6-axis IMU driver — gesture/wake/tilt (I2C 0x6A/0x6B)
- [ ] PCF85063 RTC driver — timestamps + deep sleep wake (I2C 0x51)
- [ ] AXP2101 PMIC driver — precise battery, power modes
- [ ] Buzzer PWM driver
- [ ] OTA firmware update over WiFi

---

## [0.1.0] — 2026-06-05

### Added
- EC11 rotary encoder driver (ISR + ADC button)
- WiFi Manager (NVS multi-profile, auto-reconnect, PMF compat)
- BLE WiFi provisioning service (custom UUID)
- ESPAsyncWebServer REST API (/api/status, /api/wifi, /api/led, etc.)
- LittleFS mount + static web file serving
- WS2812 RGB LED status indicator
- Battery ADC read

---

## [0.0.1] — 2026-06-04

### Added
- Initial project scaffold (PlatformIO + ESP32-S3 Arduino)
- hw/ hardware abstraction layer skeleton
- Dual board support: Waveshare / P169H002-CTP
