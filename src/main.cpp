// DeskPet ESP32-S3 — WiFi + BLE + Web Server + Status Screen
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "hw/pins.h"
#include "hw/input.h"
#include "hw/display.h"
#include "hw/power.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "stats.h"

// ── Stubs for excluded modules ─────────────────────────────────────────
// ble_bridge (NUS BLE) — not used in this test, use custom BLE instead
int  bleAvailable()           { return 0; }
int  bleRead()                { return -1; }
void bleWrite(const uint8_t*, int) {}
bool bleConnected()           { return false; }
bool bleSecure()              { return false; }
void bleClearBonds()          {}

// buddy / character — excluded, web_server references buddySetSpeciesIdx
void buddyInit()              {}
void buddyTick(uint8_t)       {}
void buddySetSpeciesIdx(uint8_t) {}
bool characterInit(const char*) { return false; }
void characterTick()          {}
void characterClose()         {}

// stats.h external refs
bool buddyMode    = true;
bool gifAvailable = false;
uint32_t _clkLastRead = 0;

// ── BLE UUIDs (custom, same as verified test) ──────────────────────────
#define SERVICE_UUID  "12345678-1234-1234-1234-123456789abc"
#define CHAR_INFO     "abcd0001-1234-1234-1234-123456789abc"
#define CHAR_WIFI_CFG "abcd0002-1234-1234-1234-123456789abc"

// ── LED ────────────────────────────────────────────────────────────────
static Adafruit_NeoPixel s_led(WS2812_NUM, PIN_WS2812, NEO_RGB + NEO_KHZ800);

static void ledSet(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 32) {
  s_led.setBrightness(brightness);
  s_led.fill(s_led.Color(r, g, b));
  s_led.show();
}

static void ledBreath(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t now = millis();
  float phase = (now % 2000) / 2000.0f;
  uint8_t bri = 4 + (uint8_t)((sin(phase * 2 * PI) + 1) * 10);
  ledSet(r, g, b, bri);
}

// ── BLE callbacks ──────────────────────────────────────────────────────
static bool s_bleConnected = false;

class ConnCallback : public BLEServerCallbacks {
  void onConnect(BLEServer*)    { s_bleConnected = true;  Serial.println("BLE: connected"); }
  void onDisconnect(BLEServer*) { s_bleConnected = false; Serial.println("BLE: disconnected"); }
};

class WifiCfgCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* chr) {
    std::string val = chr->getValue();
    if (val.empty()) return;

    // Parse {"ssid":"...","pass":"..."}
    // UTF-8 safe: strchr(s, '"') scans for byte 0x22 which never appears
    // inside multi-byte UTF-8 sequences (continuation bytes are 0x80-0xBF).
    // Sender must use json.dumps(..., ensure_ascii=False) â see stats.h.
    const char* s = strstr(val.c_str(), "\"ssid\"");
    const char* p = strstr(val.c_str(), "\"pass\"");
    if (!s || !p) { Serial.println("WiFi: bad JSON"); return; }

    char ssid[33] = {0}, pass[65] = {0};

    s = strchr(s + 6, '"'); if (!s) return;
    s++;
    const char* e = strchr(s, '"'); if (!e) return;
    size_t len = e - s; if (len >= 32) len = 31;
    memcpy(ssid, s, len);

    p = strchr(p + 6, '"'); if (!p) return;
    p++;
    e = strchr(p, '"'); if (!e) return;
    len = e - p; if (len >= 64) len = 63;
    memcpy(pass, p, len);

    Serial.printf("WiFi: provisioning '%s' via BLE\n", ssid);
    wifiMgrConnect(ssid, pass);
  }
};

// ── Status Screen ─────────────────────────────────────────────────────
// 240x260 canvas, fills the 1.69" ST7789V visible area
// Layout: title + WiFi (large) | Clawd (large) | Stats (medium) | System (small)
static void drawStatus() {
  Arduino_Canvas* c = hwCanvas();
  if (!c) return;

  // ── Title bar (text size 2, 12x16) ──
  c->fillRect(0, 0, HW_W, 26, 0x001F);
  c->setTextColor(WHITE);
  c->setTextSize(2);
  c->setCursor(6, 4);
  c->print("DeskPet S3");

  // Clawd badge (right side of title bar)
  const char* cs = webClawdState();
  uint16_t badgeBg = 0x3186;
  if      (strcmp(cs, "thinking") == 0) badgeBg = 0xFC00;
  else if (strcmp(cs, "working")  == 0) badgeBg = 0x07E0;
  else if (strcmp(cs, "juggling") == 0) badgeBg = 0x07FF;
  int csW = strlen(cs) * 12; // text size 2 = 12px wide
  c->fillRect(HW_W - csW - 12, 3, csW + 8, 20, badgeBg);
  c->setTextColor(badgeBg == 0x3186 ? WHITE : BLACK);
  c->setCursor(HW_W - csW - 8, 5);
  c->print(cs);

  int y = 30;

  // ── WiFi SSID (text size 2, green) ──
  WifiMgrState st = wifiMgrState();
  if (st == WM_OK) {
    c->setTextSize(2);
    c->setTextColor(0x07E0); c->setCursor(6, y);
    c->print(WiFi.SSID());
    y += 20;

    // IP (text size 2) + RSSI (text size 1, right-aligned)
    c->setTextColor(WHITE); c->setCursor(6, y);
    c->print(wifiMgrLocalIP());
    c->setTextSize(1);
    c->setTextColor(0xCE79);
    int rssiW = snprintf(nullptr, 0, "RSSI %d dBm", wifiMgrRssi());
    c->setCursor(HW_W - rssiW * 6 - 6, y + 3);
    c->printf("RSSI %d dBm", wifiMgrRssi());
    y += 22;
  } else {
    c->setTextSize(2);
    c->setTextColor(0xFC00); c->setCursor(6, y);
    c->print(st == WM_IDLE ? "No WiFi" :
             st == WM_CONNECTING ? "Connecting..." :
             st == WM_FAIL ? "WiFi FAILED" : "?");
    y += 24;
  }

  // ── Divider ──
  c->drawFastHLine(6, y, HW_W - 12, 0x4208); y += 8;

  // ── Clawd state (text size 2, big & centered) ──
  c->setTextSize(2);
  uint16_t csColor = 0xCE79;
  if      (strcmp(cs, "thinking") == 0) csColor = 0xFC00;
  else if (strcmp(cs, "working")  == 0) csColor = 0x07E0;
  else if (strcmp(cs, "juggling") == 0) csColor = 0x07FF;
  c->setTextColor(csColor);
  int csX = (HW_W - strlen(cs) * 12) / 2;
  c->setCursor(csX, y);
  c->print(cs);
  y += 22;

  // ── Divider ──
  c->drawFastHLine(6, y, HW_W - 12, 0x4208); y += 8;

  // ── Stats (text size 2 for key numbers, size 1 for labels) ──
  const auto& sr = stats();
  c->setTextSize(2);
  c->setTextColor(WHITE); c->setCursor(6, y);
  c->printf("Level %u", sr.level);
  c->setCursor(HW_W / 2, y);
  c->printf("Tokens %lu", (unsigned long)sr.tokens);
  y += 22;

  c->setTextSize(2);
  c->setTextColor(0xCE79); c->setCursor(6, y);
  c->printf("Approvals %u", sr.approvals);
  c->setCursor(HW_W / 2, y);
  c->printf("Denials %u", sr.denials);
  y += 20;

  uint16_t vel = statsMedianVelocity();
  c->setTextSize(1);
  c->setTextColor(0xCE79); c->setCursor(6, y);
  if (vel) c->printf("Velocity %us", vel);
  else     c->print("Velocity --");
  y += 14;

  // ── Divider ──
  c->drawFastHLine(6, y, HW_W - 12, 0x4208); y += 8;

  // ── System info (text size 1, two-column) ──
  c->setTextSize(1);
  uint32_t uptime = millis() / 1000;
  c->setTextColor(0xCE79); c->setCursor(6, y);
  c->printf("Heap %uK free", ESP.getFreeHeap() / 1024);
  c->setCursor(HW_W / 2, y);
  c->printf("%uh %um", uptime / 3600, (uptime % 3600) / 60);
  y += 14;

  // Battery
  HwBattery bat = hwBattery();
  if (bat.mV > 0) {
    c->setTextColor(0xCE79); c->setCursor(6, y);
    c->printf("Bat %d%%  %dmV", bat.pct, bat.mV);
    if (bat.usbPresent) { c->setTextColor(0x07E0); c->print(" USB"); }
    // Battery bar
    y += 10;
    int barW = HW_W - 12;
    int barPixels = (barW * bat.pct) / 100;
    uint16_t barColor = bat.pct > 50 ? 0x07E0 : (bat.pct > 20 ? 0xFC00 : 0xC104);
    c->drawRect(6, y, barW, 6, 0x7BEF);
    if (barPixels > 0) c->fillRect(6, y, barPixels, 6, barColor);
    y += 10;
  } else {
    c->setTextColor(0xCE79); c->setCursor(6, y);
    c->print("Bat --");
    y += 14;
  }

  // Flash
  c->setTextColor(0x3186); c->setCursor(6, y);
  c->printf("Flash %.1f / %.1f MB",
    LittleFS.usedBytes() / 1048576.0f, LittleFS.totalBytes() / 1048576.0f);

  // ── Footer ──
  c->setTextSize(1);
  c->setTextColor(0x3186);
  c->setCursor(6, HW_H - 10);
  c->print(petName());
  c->setCursor(HW_W / 2, HW_H - 10);
  c->print("deskpet.local");
}
void setup() {
  // 1. Serial
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== DeskPet MinTest: WiFi+BLE+Web ===");

  // 2. LED init
  s_led.begin();
  ledSet(255, 0, 0, 32);  // Red = booting

  // 3. BLE custom service
  BLEDevice::init("DeskPet-Mini");
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ConnCallback());

  BLEService* svc = server->createService(SERVICE_UUID);
  BLECharacteristic* info = svc->createCharacteristic(
    CHAR_INFO, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  info->addDescriptor(new BLE2902());
  info->setValue("{\"ssid\":\"...\",\"pass\":\"...\"}");

  BLECharacteristic* cfg = svc->createCharacteristic(
    CHAR_WIFI_CFG, BLECharacteristic::PROPERTY_WRITE);
  cfg->setCallbacks(new WifiCfgCallback());

  svc->start();
  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->start();
  Serial.println("BLE: advertising 'DeskPet-Mini'");

  // 4. WiFi manager (auto-connect if NVS has creds)
  wifiMgrInit();

  // 5. Web server (async — serves REST API when WiFi connects)
  webServerInit();

  // 6. Display — ST7789V + Canvas
  if (hwDisplayInit()) {
    Arduino_Canvas* c = hwCanvas();
    c->fillScreen(BLACK);
    c->setTextColor(WHITE);
    c->setTextSize(2);
    c->setCursor(10, 100);
    c->print("DeskPet");
    c->setTextSize(1);
    c->setCursor(10, 130);
    c->print("WiFi+BLE+Web");
    hwDisplayPush();
    Serial.println("Display: OK");
  } else {
    Serial.println("Display: FAILED");
  }

  // 7. Input — EC11 encoder
  hwInputInit();

  // 8. Ready
  Serial.println("=== Ready ===");
  ledSet(0, 0, 255, 32);  // Blue = waiting for WiFi
}

// ── Loop ───────────────────────────────────────────────────────────────
void loop() {
  static uint32_t lastLed = 0;
  uint32_t now = millis();

  // 1. WiFi state machine
  wifiMgrTick();

  // 2. Input — poll encoder
  hwInputUpdate();

  // 3. Permission system — tick timeout + handle encoder/button
  permTick();
  permInput();

  // 4. LED update + Display (500ms interval)
  if (now - lastLed > 500) {
    lastLed = now;

    // --- Display update ---
    Arduino_Canvas* c = hwCanvas();
    if (c) {
      c->fillScreen(BLACK);

      if (permActive()) {
        // Permission dialog takes over the screen
        permDraw();
      } else {
        drawStatus();
      }
      hwDisplayPush();
    }

    // --- LED ---
    const char* cs = webClawdState();
    WifiMgrState st = wifiMgrState();
    if (st != WM_OK) {
      switch (st) {
        case WM_IDLE:         ledBreath(0, 0, 255);   break; // Blue breath = waiting
        case WM_AUTO_CONNECT:
        case WM_CONNECTING:   ledSet(255, 255, 0, 32); break; // Yellow = connecting
        case WM_FAIL:         ledBreath(255, 0, 0);    break; // Red breath = fail
      }
    } else if (strcmp(cs, "thinking") == 0) {
      ledBreath(255, 165, 0);    // Orange breath = thinking
    } else if (strcmp(cs, "working") == 0) {
      ledBreath(0, 255, 0);      // Green breath = working
    } else if (strcmp(cs, "juggling") == 0) {
      ledBreath(0, 255, 255);    // Cyan breath = juggling
    } else {
      ledBreath(0, 0, 255);      // Blue breath = idle
    }
    // Log state changes
    static WifiMgrState lastSt = WM_IDLE;
    if (st != lastSt) {
      if (st == WM_OK) webLog("WiFi connected: %s RSSI=%d", wifiMgrLocalIP().c_str(), wifiMgrRssi());
      if (st == WM_FAIL) webLog("WiFi connect failed");
      lastSt = st;
    }
  }

  delay(30);
}
