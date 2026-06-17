#include <Arduino.h>
#line 1 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "lv_conf.h"
#include "HWCDC.h"
#include <WiFi.h>
#include <Preferences.h>

HWCDC USBSerial;

// ═══════════════════════════════════════════════════════════════════════════
// Display
// ═══════════════════════════════════════════════════════════════════════════
Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT,
                                      0, 20, 0, 0);

// ═══════════════════════════════════════════════════════════════════════════
// Touch (CST816T @ I2C 0x15)
// ═══════════════════════════════════════════════════════════════════════════
std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
    std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);
void Arduino_IIC_Touch_Interrupt(void);
std::unique_ptr<Arduino_IIC> CST816T(new Arduino_CST816x(
    IIC_Bus, CST816T_DEVICE_ADDRESS, TP_RST, TP_INT, Arduino_IIC_Touch_Interrupt));
#line 36 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void dispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
#line 46 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void lvglTick(void *arg);
#line 48 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void touchRead(lv_indev_drv_t *drv, lv_indev_data_t *data);
#line 70 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void lvLog(const char *buf);
#line 89 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void wfLoad();
#line 101 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void wfSave();
#line 112 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void wfConnect(int idx);
#line 119 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void wfProvision(const char *ssid, const char *pass);
#line 141 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void wfTick();
#line 154 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void wfEvent(arduino_event_id_t ev, arduino_event_info_t info);
#line 181 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static String _readLine(WiFiClient &c);
#line 186 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _sendJson(WiFiClient &c, int code, const String &body);
#line 194 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void webHandle();
#line 289 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _goHome();
#line 299 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void updateHomeUI();
#line 319 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void onSetupWifi(lv_event_t *e);
#line 330 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _populateScanList(lv_obj_t *list, int n);
#line 353 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void onManualWifi(lv_event_t *e);
#line 362 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _onPassDone(lv_event_t *e);
#line 374 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _onManualSwitchToPass(lv_event_t *e);
#line 381 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _onManualSwitchToSsid(lv_event_t *e);
#line 386 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void _onManualDone(lv_event_t *e);
#line 401 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void permShow();
#line 410 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void permApprove(lv_event_t *e);
#line 416 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void permDeny(lv_event_t *e);
#line 422 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void permTick();
#line 438 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void buildHomeScreen();
#line 465 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void buildWifiScanScreen();
#line 490 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void buildWifiPassScreen();
#line 523 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void buildWifiManualScreen();
#line 579 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
static void buildPermScreen();
#line 628 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void setup();
#line 715 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void loop();
#line 27 "C:\\Users\\ROG\\deskpet-firmware\\test\\waveshare_s3\\waveshare_s3.ino"
void Arduino_IIC_Touch_Interrupt(void) { CST816T->IIC_Interrupt_Flag = true; }

// ═══════════════════════════════════════════════════════════════════════════
// LVGL
// ═══════════════════════════════════════════════════════════════════════════
#define LVGL_TICK_MS 2
uint32_t screenW, screenH;
static lv_disp_draw_buf_t draw_buf;

void dispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = area->x2 - area->x1 + 1, h = area->y2 - area->y1 + 1;
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
  lv_disp_flush_ready(disp);
}

void lvglTick(void *arg) { lv_tick_inc(LVGL_TICK_MS); }

void touchRead(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  Wire.beginTransmission(0x15);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(0x15, 1);
  uint8_t fingers = Wire.read();
  if (fingers > 0 && fingers <= 2) {
    int32_t tx = CST816T->IIC_Read_Device_Value(
        CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t ty = CST816T->IIC_Read_Device_Value(
        CST816T->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);
    if (tx >= 0 && ty >= 0) {
      data->state = LV_INDEV_STATE_PR;
      data->point.x = tx;
      data->point.y = ty;
    }
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

#if LV_USE_LOG != 0
void lvLog(const char *buf) { Serial.printf(buf); Serial.flush(); }
#endif

// ═══════════════════════════════════════════════════════════════════════════
// WiFi Manager (NVS-backed, 5 profiles, auto-connect + retry)
// ═══════════════════════════════════════════════════════════════════════════
#define WIFI_MAX 5
#define WIFI_TIMEOUT 15000

static char     wfSsid[WIFI_MAX][33] = {{0}};
static char     wfPass[WIFI_MAX][65] = {{0}};
static uint8_t  wfCount = 0;
static int8_t   wfCur = -1;
static bool     wfOk = false;
static String   wfIP = "";
static int      wfRssi = 0;
static uint32_t wfStart = 0;
static Preferences wfPrefs;

static void wfLoad() {
  wfPrefs.begin("deskpet", true);
  wfCount = wfPrefs.getUChar("w_n", 0);
  if (wfCount > WIFI_MAX) wfCount = WIFI_MAX;
  for (int i = 0; i < wfCount; i++) {
    char k[8];
    snprintf(k, sizeof(k), "w_s%d", i); wfPrefs.getString(k, wfSsid[i], 33);
    snprintf(k, sizeof(k), "w_p%d", i); wfPrefs.getString(k, wfPass[i], 65);
  }
  wfPrefs.end();
}

static void wfSave() {
  wfPrefs.begin("deskpet", false);
  wfPrefs.putUChar("w_n", wfCount);
  for (int i = 0; i < wfCount; i++) {
    char k[8];
    snprintf(k, sizeof(k), "w_s%d", i); wfPrefs.putString(k, wfSsid[i]);
    snprintf(k, sizeof(k), "w_p%d", i); wfPrefs.putString(k, wfPass[i]);
  }
  wfPrefs.end();
}

void wfConnect(int idx) {
  if (idx < 0 || idx >= wfCount) return;
  wfCur = idx; wfStart = millis();
  USBSerial.printf("[WiFi] Connecting to '%s'...\n", wfSsid[idx]);
  WiFi.begin(wfSsid[idx], wfPass[idx]);
}

void wfProvision(const char *ssid, const char *pass) {
  if (!ssid || !ssid[0]) return;
  int found = -1;
  for (int i = 0; i < wfCount; i++) { if (strcmp(wfSsid[i], ssid) == 0) { found = i; break; } }
  if (found > 0) {
    char s[33], p[65]; strncpy(s, wfSsid[found], 32); strncpy(p, wfPass[found], 64);
    for (int j = found; j > 0; j--) {
      strncpy(wfSsid[j], wfSsid[j-1], 32); strncpy(wfPass[j], wfPass[j-1], 64);
    }
    strncpy(wfSsid[0], s, 32); strncpy(wfPass[0], p, 64);
  } else {
    int n = wfCount < WIFI_MAX ? wfCount : WIFI_MAX - 1;
    for (int j = n; j > 0; j--) {
      strncpy(wfSsid[j], wfSsid[j-1], 32); strncpy(wfPass[j], wfPass[j-1], 64);
    }
    strncpy(wfSsid[0], ssid, 32); wfSsid[0][32] = 0;
    strncpy(wfPass[0], pass ? pass : "", 64); wfPass[0][64] = 0;
    if (wfCount < WIFI_MAX) wfCount++;
  }
  wfSave(); WiFi.disconnect(); delay(100); wfConnect(0);
}

void wfTick() {
  uint32_t now = millis();
  if (wfCur < 0 || wfOk) return;
  if (now - wfStart > WIFI_TIMEOUT) {
    USBSerial.printf("[WiFi] Timeout: '%s'\n", wfSsid[wfCur]);
    WiFi.disconnect();
    if (wfCur + 1 < wfCount) {
      wfCur++; wfStart = now;
      WiFi.begin(wfSsid[wfCur], wfPass[wfCur]);
    } else { wfCur = -1; USBSerial.println("[WiFi] All profiles exhausted"); }
  }
}

void wfEvent(WiFiEvent_t ev, WiFiEventInfo_t info) {
  if (ev == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
    wfIP = IPAddress(info.got_ip.ip_info.ip.addr).toString();
    wfRssi = WiFi.RSSI(); wfOk = true;
    USBSerial.printf("[WiFi] OK IP:%s RSSI:%d\n", wfIP.c_str(), wfRssi);
  } else if (ev == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
    uint8_t reason = info.wifi_sta_disconnected.reason;
    wfOk = false; wfIP = "";
    USBSerial.printf("[WiFi] Disconnected reason=%d\n", reason);
    if (wfCur >= 0) { wfStart = millis(); USBSerial.println("[WiFi] Retrying..."); }
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// Web Server (lightweight WiFiServer-based, no AsyncWebServer)
// ═══════════════════════════════════════════════════════════════════════════
WiFiServer webServer(80);

struct PermReq {
  bool pending = false;
  char id[40] = {0};
  char tool[20] = {0};
  char hint[50] = {0};
  int8_t result = -1;  // -1=pending, 0=deny, 1=approve
};
static PermReq s_perm;

static String _readLine(WiFiClient &c) {
  String s; while (c.connected() && c.available()) { char ch = c.read(); if (ch == '\n') break; if (ch != '\r') s += ch; }
  return s;
}

static void _sendJson(WiFiClient &c, int code, const String &body) {
  c.printf("HTTP/1.1 %d OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\nContent-Length: %d\r\n\r\n%s", code, body.length(), body.c_str());
}

static void _sendEmpty(WiFiClient &c, int code, const char *extraHeaders = "") {
  c.printf("HTTP/1.1 %d OK\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n%s\r\n", code, extraHeaders);
}

void webHandle() {
  WiFiClient client = webServer.available();
  if (!client) return;

  String method, path, body;
  int contentLen = 0;
  uint32_t t0 = millis();

  // Read request line
  while (client.connected() && millis() - t0 < 1000) {
    if (client.available()) {
      String line = _readLine(client);
      if (method.isEmpty()) {  // first line: GET /path HTTP/1.1
        int s1 = line.indexOf(' '), s2 = line.indexOf(' ', s1 + 1);
        if (s1 >= 0) { method = line.substring(0, s1); path = s2 > s1 ? line.substring(s1 + 1, s2) : line.substring(s1 + 1); }
      } else if (line.startsWith("Content-Length:")) {
        contentLen = line.substring(15).toInt();
      } else if (line.isEmpty()) {
        break;  // end of headers
      }
      t0 = millis();
    }
  }

  // Read body if present
  if (contentLen > 0) {
    while (client.connected() && body.length() < (size_t)contentLen && millis() - t0 < 1000) {
      if (client.available()) { body += (char)client.read(); t0 = millis(); }
    }
  }

  // ── Route ──
  if (method == "OPTIONS") {
    _sendEmpty(client, 204, "Allow: GET, POST, OPTIONS\r\n");
  }
  else if (path == "/api/status" || path.startsWith("/api/status?")) {
    String json = "{\"name\":\"DeskPet-S3\",\"ip\":\"" + wfIP + "\",\"rssi\":" + String(wfRssi) +
                  ",\"free_heap\":" + String(ESP.getFreeHeap()) +
                  ",\"permission_pending\":" + String(s_perm.pending ? "true" : "false") + "}";
    _sendJson(client, 200, json);
  }
  else if (path == "/api/permission" && method == "POST") {
    // Parse {"id":"...","tool":"...","hint":"..."}
    const char *s = strstr(body.c_str(), "\"id\"");
    const char *t = strstr(body.c_str(), "\"tool\"");
    const char *h = strstr(body.c_str(), "\"hint\"");
    if (!s || !t || !h) { _sendJson(client, 400, "{\"error\":\"bad json\"}"); client.stop(); return; }

    auto extract = [](const char *p, char *dst, size_t max) {
      p = strchr(p, '"'); if (!p) return; p++;
      const char *e = strchr(p, '"'); if (!e) return;
      size_t n = e - p; if (n >= max) n = max - 1;
      memcpy(dst, p, n); dst[n] = 0;
    };
    extract(s + 4, s_perm.id, sizeof(s_perm.id));
    extract(t + 6, s_perm.tool, sizeof(s_perm.tool));
    extract(h + 6, s_perm.hint, sizeof(s_perm.hint));

    if (s_perm.pending) { _sendJson(client, 409, "{\"error\":\"permission already pending\"}"); client.stop(); return; }

    s_perm.pending = true; s_perm.result = -1;
    USBSerial.printf("[PERM] id=%s tool=%s hint=%s\n", s_perm.id, s_perm.tool, s_perm.hint);
    _sendJson(client, 200, "{\"accepted\":true,\"id\":\"" + String(s_perm.id) + "\"}");
  }
  else if (path.startsWith("/api/permission/result") && method == "GET") {
    int eq = path.indexOf("id=");
    String qid = eq >= 0 ? path.substring(eq + 3) : "";
    if (s_perm.pending) {
      _sendJson(client, 200, "{\"id\":\"" + String(s_perm.id) + "\",\"pending\":true}");
    } else if (!qid.isEmpty() && qid == String(s_perm.id)) {
      _sendJson(client, 200, "{\"id\":\"" + String(s_perm.id) + "\",\"approved\":" +
                             String(s_perm.result == 1 ? "true" : "false") + "}");
      s_perm.id[0] = 0;  // clear after retrieval
    } else {
      _sendJson(client, 404, "{\"error\":\"no result\"}");
    }
  }
  else {
    _sendJson(client, 404, "{\"error\":\"not found\"}");
  }
  client.stop();
}

// ═══════════════════════════════════════════════════════════════════════════
// LVGL Screens
// ═══════════════════════════════════════════════════════════════════════════
enum Screen { SCR_HOME, SCR_WIFI_SCAN, SCR_WIFI_PASS, SCR_WIFI_MANUAL, SCR_PERM };
static Screen scrCurrent = SCR_HOME;

// Forward decls
static lv_obj_t *scrHome, *scrWifiScan, *scrWifiPass, *scrWifiManual, *scrPerm;
static lv_obj_t *kb = NULL;  // shared keyboard
static char wifiSelSsid[33] = {0};

// ── Navigation ─────────────────────────────────────────────────────────
static void _goHome() {
  if (kb) { lv_obj_del(kb); kb = NULL; }
  lv_scr_load(scrHome);
  scrCurrent = SCR_HOME;
  updateHomeUI();
}

// ── Home Screen ────────────────────────────────────────────────────────
static lv_obj_t *homeTitle, *homeWifi, *homeIP, *homeBtnLabel;

void updateHomeUI() {
  if (wfOk) {
    lv_label_set_text(homeWifi, "WiFi: Connected");
    lv_label_set_text_fmt(homeIP, "IP: %s", wfIP.c_str());
  } else if (wfCur >= 0 && wfCur < wfCount) {
    lv_label_set_text_fmt(homeWifi, "Connecting to %s...", wfSsid[wfCur]);
    lv_label_set_text(homeIP, "");
  } else if (wfCount == 0) {
    lv_label_set_text(homeWifi, "WiFi: Not configured");
    lv_label_set_text(homeIP, "Tap below to set up");
  } else {
    lv_label_set_text(homeWifi, "WiFi: Connection failed");
    lv_label_set_text(homeIP, "");
  }
}

// Async WiFi scan state
static bool _scanActive = false;
static int  _scanResult = -2;  // -2=not started, -1=scanning, >=0 count

static void onSetupWifi(lv_event_t *e) {
  lv_scr_load(scrWifiScan);
  scrCurrent = SCR_WIFI_SCAN;
  lv_obj_t *list = lv_obj_get_child(scrWifiScan, 0);
  lv_obj_clean(list);
  lv_list_add_text(list, "Scanning...");
  _scanActive = true;
  _scanResult = -1;
  WiFi.scanNetworks(true);  // async scan
}

static void _populateScanList(lv_obj_t *list, int n) {
  lv_obj_clean(list);
  for (int i = 0; i < n; i++) {
    String label = WiFi.SSID(i);
    label += "  ("; label += WiFi.RSSI(i); label += "dBm)";
    if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) label += " *";
    lv_obj_t *btn = lv_list_add_btn(list, NULL, label.c_str());
    // Store SSID index, not string pointer
    lv_obj_set_user_data(btn, (void *)(intptr_t)i);
    lv_obj_add_event_cb(btn, [](lv_event_t *ev) {
      int idx = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_target(ev));
      String ssid = WiFi.SSID(idx);
      strncpy(wifiSelSsid, ssid.c_str(), 32); wifiSelSsid[32] = 0;
      lv_label_set_text_fmt(lv_obj_get_child(scrWifiPass, 0), "SSID: %s", wifiSelSsid);
      lv_textarea_set_text(lv_obj_get_child(scrWifiPass, 1), "");
      lv_scr_load(scrWifiPass);
      scrCurrent = SCR_WIFI_PASS;
      if (!kb) { kb = lv_keyboard_create(scrWifiPass); lv_keyboard_set_textarea(kb, lv_obj_get_child(scrWifiPass, 1)); }
    }, LV_EVENT_CLICKED, NULL);
  }
  if (n == 0) lv_list_add_text(list, "No networks found");
}

static void onManualWifi(lv_event_t *e) {
  lv_scr_load(scrWifiManual);
  scrCurrent = SCR_WIFI_MANUAL;
  lv_textarea_set_text(lv_obj_get_child(scrWifiManual, 0), "");
  lv_textarea_set_text(lv_obj_get_child(scrWifiManual, 1), "");
  if (!kb) { kb = lv_keyboard_create(scrWifiManual); lv_keyboard_set_textarea(kb, lv_obj_get_child(scrWifiManual, 0)); }
}

// ── WiFi Password Screen ───────────────────────────────────────────────
static void _onPassDone(lv_event_t *e) {
  const char *pass = lv_textarea_get_text(lv_obj_get_child(scrWifiPass, 1));
  USBSerial.printf("[WiFi] Provisioning: '%s'\n", wifiSelSsid);
  wfProvision(wifiSelSsid, pass);
  if (kb) { lv_obj_del(kb); kb = NULL; }
  _goHome();
}

// ── WiFi Manual Screen ─────────────────────────────────────────────────
static lv_obj_t *_manualSsidArea, *_manualPassArea;
static bool _manualEditingSsid = true;

static void _onManualSwitchToPass(lv_event_t *e) {
  const char *ssid = lv_textarea_get_text(_manualSsidArea);
  if (ssid[0] == 0) return;
  _manualEditingSsid = false;
  lv_keyboard_set_textarea(kb, _manualPassArea);
}

static void _onManualSwitchToSsid(lv_event_t *e) {
  _manualEditingSsid = true;
  lv_keyboard_set_textarea(kb, _manualSsidArea);
}

static void _onManualDone(lv_event_t *e) {
  const char *ssid = lv_textarea_get_text(_manualSsidArea);
  const char *pass = lv_textarea_get_text(_manualPassArea);
  if (ssid[0] == 0) return;
  USBSerial.printf("[WiFi] Manual provision: '%s'\n", ssid);
  wfProvision(ssid, pass);
  if (kb) { lv_obj_del(kb); kb = NULL; }
  _goHome();
}

// ── Permission Screen ──────────────────────────────────────────────────
static lv_obj_t *permTool, *permHint, *permCountdown;
static uint32_t permStart = 0;
static const uint32_t PERM_TIMEOUT = 60000;

void permShow() {
  lv_label_set_text_fmt(permTool, "Tool: %s", s_perm.tool);
  lv_label_set_text_fmt(permHint, "%s", s_perm.hint);
  lv_label_set_text(permCountdown, "60s");
  permStart = millis();
  lv_scr_load(scrPerm);
  scrCurrent = SCR_PERM;
}

void permApprove(lv_event_t *e) {
  s_perm.result = 1; s_perm.pending = false;
  USBSerial.printf("[PERM] APPROVED id=%s\n", s_perm.id);
  _goHome();
}

void permDeny(lv_event_t *e) {
  s_perm.result = 0; s_perm.pending = false;
  USBSerial.printf("[PERM] DENIED id=%s\n", s_perm.id);
  _goHome();
}

void permTick() {
  if (!s_perm.pending) return;
  uint32_t elapsed = millis() - permStart;
  if (elapsed > PERM_TIMEOUT) {
    s_perm.result = 0; s_perm.pending = false;
    USBSerial.printf("[PERM] TIMEOUT id=%s\n", s_perm.id);
    _goHome();
    return;
  }
  int remain = (PERM_TIMEOUT - elapsed) / 1000;
  lv_label_set_text_fmt(permCountdown, "%ds", remain);
}

// ═══════════════════════════════════════════════════════════════════════════
// Screen Builders
// ═══════════════════════════════════════════════════════════════════════════
static void buildHomeScreen() {
  scrHome = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scrHome, lv_color_black(), 0);

  homeTitle = lv_label_create(scrHome);
  lv_label_set_text(homeTitle, "DeskPet S3");
  lv_obj_set_style_text_color(homeTitle, lv_color_white(), 0);
  lv_obj_align(homeTitle, LV_ALIGN_TOP_MID, 0, 5);

  homeWifi = lv_label_create(scrHome);
  lv_obj_set_style_text_color(homeWifi, lv_color_hex(0xCCCCCC), 0);
  lv_obj_align(homeWifi, LV_ALIGN_TOP_LEFT, 5, 35);

  homeIP = lv_label_create(scrHome);
  lv_obj_set_style_text_color(homeIP, lv_color_hex(0x00FF00), 0);
  lv_obj_align(homeIP, LV_ALIGN_TOP_LEFT, 5, 55);

  // Setup WiFi button
  lv_obj_t *btn = lv_btn_create(scrHome);
  lv_obj_set_size(btn, 140, 36);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  homeBtnLabel = lv_label_create(btn);
  lv_label_set_text(homeBtnLabel, "Setup WiFi");
  lv_obj_center(homeBtnLabel);
  lv_obj_add_event_cb(btn, onSetupWifi, LV_EVENT_CLICKED, NULL);
}

static void buildWifiScanScreen() {
  scrWifiScan = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scrWifiScan, lv_color_black(), 0);

  lv_obj_t *list = lv_list_create(scrWifiScan);
  lv_obj_set_size(list, 230, 200);
  lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 10);

  // Manual entry button
  lv_obj_t *btn = lv_btn_create(scrWifiScan);
  lv_obj_set_size(btn, 100, 30);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
  lv_label_set_text(lv_label_create(btn), "Manual");
  lv_obj_center(lv_obj_get_child(btn, 0));
  lv_obj_add_event_cb(btn, onManualWifi, LV_EVENT_CLICKED, NULL);

  // Back button
  btn = lv_btn_create(scrWifiScan);
  lv_obj_set_size(btn, 60, 30);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);
  lv_label_set_text(lv_label_create(btn), "Back");
  lv_obj_center(lv_obj_get_child(btn, 0));
  lv_obj_add_event_cb(btn, [](lv_event_t *e) { _goHome(); }, LV_EVENT_CLICKED, NULL);
}

static void buildWifiPassScreen() {
  scrWifiPass = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scrWifiPass, lv_color_black(), 0);

  // SSID label
  lv_obj_t *label = lv_label_create(scrWifiPass);
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 5);

  // Password textarea
  lv_obj_t *ta = lv_textarea_create(scrWifiPass);
  lv_obj_set_size(ta, 230, 40);
  lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 30);
  lv_textarea_set_password_mode(ta, true);
  lv_textarea_set_max_length(ta, 63);
  lv_textarea_set_placeholder_text(ta, "Password");

  // Done button
  lv_obj_t *btn = lv_btn_create(scrWifiPass);
  lv_obj_set_size(btn, 80, 30);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
  lv_label_set_text(lv_label_create(btn), "Connect");
  lv_obj_center(lv_obj_get_child(btn, 0));
  lv_obj_add_event_cb(btn, _onPassDone, LV_EVENT_CLICKED, NULL);

  // Back button
  btn = lv_btn_create(scrWifiPass);
  lv_obj_set_size(btn, 60, 30);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);
  lv_label_set_text(lv_label_create(btn), "Back");
  lv_obj_center(lv_obj_get_child(btn, 0));
  lv_obj_add_event_cb(btn, [](lv_event_t *e) { if (kb) { lv_obj_del(kb); kb = NULL; } lv_scr_load(scrWifiScan); scrCurrent = SCR_WIFI_SCAN; }, LV_EVENT_CLICKED, NULL);
}

static void buildWifiManualScreen() {
  scrWifiManual = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scrWifiManual, lv_color_black(), 0);

  // SSID
  lv_obj_t *label = lv_label_create(scrWifiManual);
  lv_label_set_text(label, "SSID:");
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 5);
  _manualSsidArea = lv_textarea_create(scrWifiManual);
  lv_obj_set_size(_manualSsidArea, 230, 36);
  lv_obj_align(_manualSsidArea, LV_ALIGN_TOP_MID, 0, 25);
  lv_textarea_set_max_length(_manualSsidArea, 32);
  lv_textarea_set_placeholder_text(_manualSsidArea, "Network name");

  // Password
  label = lv_label_create(scrWifiManual);
  lv_label_set_text(label, "Password:");
  lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, 70);
  _manualPassArea = lv_textarea_create(scrWifiManual);
  lv_obj_set_size(_manualPassArea, 230, 36);
  lv_obj_align(_manualPassArea, LV_ALIGN_TOP_MID, 0, 90);
  lv_textarea_set_password_mode(_manualPassArea, true);
  lv_textarea_set_max_length(_manualPassArea, 63);
  lv_textarea_set_placeholder_text(_manualPassArea, "Password");

  // Switch button
  lv_obj_t *btnSwitch = lv_btn_create(scrWifiManual);
  lv_obj_set_size(btnSwitch, 120, 28);
  lv_obj_align(btnSwitch, LV_ALIGN_TOP_MID, 0, 135);
  lv_obj_t *btnLabel = lv_label_create(btnSwitch);
  lv_label_set_text(btnLabel, "Edit Password");
  lv_obj_center(btnLabel);
  lv_obj_set_user_data(btnSwitch, btnLabel);
  lv_obj_add_event_cb(btnSwitch, [](lv_event_t *e) {
    lv_obj_t *lbl = (lv_obj_t *)lv_obj_get_user_data(lv_event_get_target(e));
    if (_manualEditingSsid) { _onManualSwitchToPass(e); lv_label_set_text(lbl, "Edit SSID"); }
    else { _onManualSwitchToSsid(e); lv_label_set_text(lbl, "Edit Password"); }
  }, LV_EVENT_CLICKED, NULL);

  // Connect
  lv_obj_t *btn = lv_btn_create(scrWifiManual);
  lv_obj_set_size(btn, 80, 30);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
  lv_label_set_text(lv_label_create(btn), "Connect");
  lv_obj_center(lv_obj_get_child(btn, 0));
  lv_obj_add_event_cb(btn, _onManualDone, LV_EVENT_CLICKED, NULL);

  // Back
  btn = lv_btn_create(scrWifiManual);
  lv_obj_set_size(btn, 60, 30);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 5, -5);
  lv_label_set_text(lv_label_create(btn), "Back");
  lv_obj_center(lv_obj_get_child(btn, 0));
  lv_obj_add_event_cb(btn, [](lv_event_t *e) { if (kb) { lv_obj_del(kb); kb = NULL; } lv_scr_load(scrWifiScan); scrCurrent = SCR_WIFI_SCAN; }, LV_EVENT_CLICKED, NULL);
}

static void buildPermScreen() {
  scrPerm = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scrPerm, lv_color_hex(0x222222), 0);

  lv_obj_t *title = lv_label_create(scrPerm);
  lv_label_set_text(title, "Permission Request");
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFAA00), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  permTool = lv_label_create(scrPerm);
  lv_obj_set_style_text_color(permTool, lv_color_white(), 0);
  lv_obj_align(permTool, LV_ALIGN_TOP_LEFT, 10, 40);
  lv_label_set_long_mode(permTool, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(permTool, 220);

  permHint = lv_label_create(scrPerm);
  lv_obj_set_style_text_color(permHint, lv_color_hex(0xCCCCCC), 0);
  lv_obj_align(permHint, LV_ALIGN_TOP_LEFT, 10, 65);
  lv_label_set_long_mode(permHint, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(permHint, 220);

  permCountdown = lv_label_create(scrPerm);
  lv_obj_set_style_text_color(permCountdown, lv_color_hex(0x888888), 0);
  lv_obj_align(permCountdown, LV_ALIGN_TOP_RIGHT, -10, 10);

  // Approve button
  lv_obj_t *btnAppr = lv_btn_create(scrPerm);
  lv_obj_set_size(btnAppr, 100, 40);
  lv_obj_align(btnAppr, LV_ALIGN_BOTTOM_LEFT, 10, -30);
  lv_obj_set_style_bg_color(btnAppr, lv_color_hex(0x006600), 0);
  lv_obj_t *lbl = lv_label_create(btnAppr);
  lv_label_set_text(lbl, "Approve");
  lv_obj_center(lbl);
  lv_obj_add_event_cb(btnAppr, permApprove, LV_EVENT_CLICKED, NULL);

  // Deny button
  lv_obj_t *btnDeny = lv_btn_create(scrPerm);
  lv_obj_set_size(btnDeny, 100, 40);
  lv_obj_align(btnDeny, LV_ALIGN_BOTTOM_RIGHT, -10, -30);
  lv_obj_set_style_bg_color(btnDeny, lv_color_hex(0x660000), 0);
  lbl = lv_label_create(btnDeny);
  lv_label_set_text(lbl, "Deny");
  lv_obj_center(lbl);
  lv_obj_add_event_cb(btnDeny, permDeny, LV_EVENT_CLICKED, NULL);
}

// ═══════════════════════════════════════════════════════════════════════════
// Setup
// ═══════════════════════════════════════════════════════════════════════════
void setup() {
  USBSerial.begin(115200);
  delay(300);
  USBSerial.println("\n=== DeskPet Waveshare S3 ===");

  // Touch
  USBSerial.print("Touch: ");
  while (!CST816T->begin()) { USBSerial.println("FAIL"); delay(2000); }
  USBSerial.println("OK");
  CST816T->IIC_Write_Device_State(
      CST816T->Arduino_IIC_Touch::Device::TOUCH_DEVICE_INTERRUPT_MODE,
      CST816T->Arduino_IIC_Touch::Device_Mode::TOUCH_DEVICE_INTERRUPT_PERIODIC);

  // Display
  gfx->begin();
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  screenW = gfx->width();
  screenH = gfx->height();

  // LVGL
  lv_init();
  lv_color_t *b1 = (lv_color_t *)heap_caps_malloc(screenW * screenH / 4 * sizeof(lv_color_t), MALLOC_CAP_DMA);
  lv_color_t *b2 = (lv_color_t *)heap_caps_malloc(screenW * screenH / 4 * sizeof(lv_color_t), MALLOC_CAP_DMA);
  lv_disp_draw_buf_init(&draw_buf, b1, b2, screenW * screenH / 4);

  static lv_disp_drv_t dd; lv_disp_drv_init(&dd);
  dd.hor_res = screenW; dd.ver_res = screenH;
  dd.flush_cb = dispFlush; dd.draw_buf = &draw_buf;
  lv_disp_drv_register(&dd);

  static lv_indev_drv_t id; lv_indev_drv_init(&id);
  id.type = LV_INDEV_TYPE_POINTER; id.read_cb = touchRead;
  lv_indev_drv_register(&id);

  esp_timer_handle_t tickH = NULL;
  esp_timer_create_args_t targs = {};
  targs.callback = lvglTick;
  targs.name = "lvgl";
  esp_timer_create(&targs, &tickH);
  esp_timer_start_periodic(tickH, LVGL_TICK_MS * 1000);

#if LV_USE_LOG != 0
  lv_log_register_print_cb(lvLog);
#endif

  // Build screens
  buildHomeScreen();
  buildWifiScanScreen();
  buildWifiPassScreen();
  buildWifiManualScreen();
  buildPermScreen();
  lv_scr_load(scrHome);

  // WiFi
  wfLoad();
  // One-time: clear old creds and set OmniX_Race as default
  wfPrefs.begin("deskpet", false);
  if (!wfPrefs.getBool("init", false)) {
    wfPrefs.clear();
    wfPrefs.putBool("init", true);
    wfPrefs.end();
    wfCount = 0;
    wfProvision("OmniX_Race", "12345678");
    USBSerial.println("[WiFi] One-time init: OmniX_Race saved");
    wfLoad();
  } else {
    wfPrefs.end();
  }
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(wfEvent);
  if (wfCount > 0 && wfSsid[0][0]) {
    USBSerial.printf("[WiFi] Auto-connect '%s'\n", wfSsid[0]);
    wfConnect(0);
  } else {
    USBSerial.println("[WiFi] No saved credentials");
  }
  updateHomeUI();

  // Web server — start after WiFi connects
  // (will start on first successful connection via wfEvent)
  USBSerial.println("=== Ready ===");
}

// ═══════════════════════════════════════════════════════════════════════════
// Loop
// ═══════════════════════════════════════════════════════════════════════════
void loop() {
  static uint32_t lastUI = 0;
  static bool serverStarted = false;
  uint32_t now = millis();

  lv_timer_handler();
  wfTick();

  // Poll WiFi scan completion (async, runs in main loop = LVGL-safe)
  if (_scanActive) {
    int n = WiFi.scanComplete();
    if (n >= 0) {
      _scanResult = n;
      _scanActive = false;
      lv_obj_t *list = lv_obj_get_child(scrWifiScan, 0);
      _populateScanList(list, n);
    }
  }

  // Start web server once WiFi connects
  if (wfOk && !serverStarted) {
    webServer.begin();
    serverStarted = true;
    USBSerial.println("[Web] Server started on port 80");
  }
  if (!wfOk) serverStarted = false;

  // Handle HTTP clients
  if (serverStarted) webHandle();

  // Permission timeout check
  permTick();

  // Show permission dialog when pending and not already showing
  if (s_perm.pending && scrCurrent != SCR_PERM) {
    permShow();
  }
  // Clear permission when resolved and showing
  if (!s_perm.pending && scrCurrent == SCR_PERM) {
    _goHome();
  }

  // UI update timer
  if (now - lastUI > 2000) {
    lastUI = now;
    if (wfOk) wfRssi = WiFi.RSSI();
    if (scrCurrent == SCR_HOME) updateHomeUI();
  }

  delay(5);
}

