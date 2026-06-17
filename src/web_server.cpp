// DeskPet Web Server
#include "web_server.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "wifi_manager.h"
#include "stats.h"
#include "hw/power.h"
#include "hw/display.h"
#include "hw/input.h"
#include "hw/audio.h"

static AsyncWebServer server(80);
static bool s_webRunning = false;
static String s_clawdState = "idle";

// Log buffer
static const size_t LOG_CAP = 100;
static char   logBuf[LOG_CAP][128];
static size_t logHead  = 0;
static size_t logCount = 0;

void webLog(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(logBuf[logHead], sizeof(logBuf[0]), fmt, args);
  va_end(args);
  logHead = (logHead + 1) % LOG_CAP;
  if (logCount < LOG_CAP) logCount++;
}

String webLogGet() {
  String json = "[";
  size_t start = (logCount < LOG_CAP) ? 0 : logHead;
  for (size_t i = 0; i < logCount; i++) {
    size_t idx = (start + i) % LOG_CAP;
    if (i > 0) json += ',';
    json += '"';
    for (char* p = logBuf[idx]; *p; p++) {
      if (*p == 0x22) { json += (char)0x5C; } // escape double-quote
      if (*p == '\n') { json += "\\n"; continue; }
      if (*p == '\r') continue;
      if (*p == '\t') { json += "\\t"; continue; }
      json += *p;
    }
    json += '"';
  }
  json += ']';
  return json;
}

// CORS helper
static void addCors(AsyncWebServerResponse* resp) {
  resp->addHeader("Access-Control-Allow-Origin", "*");
  resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
}

// GET /api/status
static void handleStatus(AsyncWebServerRequest* req) {
  AsyncResponseStream* resp = req->beginResponseStream("application/json");
  JsonDocument doc;

  doc["name"]    = petName();
  doc["owner"]   = ownerName();
  doc["species"] = speciesIdxLoad();

  HwBattery bat = hwBattery();
  JsonObject batObj = doc["battery"].to<JsonObject>();
  batObj["pct"]      = bat.pct;
  batObj["mV"]       = bat.mV;
  batObj["charging"] = bat.usbPresent;

  JsonObject wifiObj = doc["wifi"].to<JsonObject>();
  wifiObj["connected"] = wifiMgrConnected();
  wifiObj["rssi"]      = wifiMgrConnected() ? wifiMgrRssi() : 0;
  wifiObj["ip"]        = wifiMgrConnected() ? wifiMgrLocalIP() : "";
  wifiObj["ssid"]      = wifiMgrConnected() ? WiFi.SSID() : "";

  JsonObject sysObj = doc["sys"].to<JsonObject>();
  sysObj["uptime_sec"] = millis() / 1000;
  sysObj["free_heap"]  = ESP.getFreeHeap();
  sysObj["free_psram"] = ESP.getFreePsram();
  sysObj["fs_used"]    = LittleFS.usedBytes();
  sysObj["fs_total"]   = LittleFS.totalBytes();

  const auto& st = stats();
  JsonObject stObj = doc["stats"].to<JsonObject>();
  stObj["approvals"]   = st.approvals;
  stObj["denials"]     = st.denials;
  stObj["velocity"]    = statsMedianVelocity();
  stObj["nap_seconds"] = st.napSeconds;
  stObj["level"]       = st.level;
  stObj["tokens"]      = st.tokens;

  extern bool buddyMode, gifAvailable;
  doc["display"] = !gifAvailable ? "pet" : (buddyMode ? "pet" : "normal");
  doc["clawd_state"] = s_clawdState;

  HwTouch t = hwTouch();
  JsonObject touchObj = doc["touch"].to<JsonObject>();
  touchObj["down"] = t.down;
  touchObj["x"]    = t.x;
  touchObj["y"]    = t.y;

  serializeJson(doc, *resp);
  addCors(resp);
  req->send(resp);
}

// POST /api/led — Clawd state → ESP32 LED color
static void onLed(AsyncWebServerRequest* req, JsonVariant& json) {
  const char* color = json["color"];
  if (color) {
    s_clawdState = color;
    Serial.printf("[led] clawd state → %s\n", color);
  }
  AsyncWebServerResponse* resp = req->beginResponse(200, "application/json", "{\"ok\":true}");
  addCors(resp);
  req->send(resp);
}

// POST /api/display
static void handleDisplay(AsyncWebServerRequest* req, JsonVariant& json) {
  const char* mode = json["mode"];
  if (!mode) { req->send(400, "application/json", "{\"ok\":false}"); return; }
  extern bool buddyMode, gifAvailable;
  if (strcmp(mode, "sleep") == 0) {
    hwDisplaySleep(true);
  } else {
    hwDisplaySleep(false);
    if (strcmp(mode, "pet") == 0) { buddyMode = true; }
    else if (strcmp(mode, "normal") == 0) { buddyMode = !gifAvailable; }
  }
  req->send(200, "application/json", "{\"ok\":true}");
}

// POST /api/brightness
static void handleBrightness(AsyncWebServerRequest* req, JsonVariant& json) {
  uint8_t lvl = json["level"] | 99;
  if (lvl > 4) { req->send(400, "application/json", "{\"ok\":false}"); return; }
  hwDisplayBrightness(lvl);
  req->send(200, "application/json", "{\"ok\":true}");
}

// POST /api/sound
static void handleSound(AsyncWebServerRequest* req, JsonVariant& json) {
  if (!json["sound"].is<bool>()) { req->send(400, "application/json", "{\"ok\":false}"); return; }
  _settings.sound = json["sound"].as<bool>();
  settingsSave();
  req->send(200, "application/json", "{\"ok\":true}");
}

// POST /api/name
static void handleName(AsyncWebServerRequest* req, JsonVariant& json) {
  const char* name = json["name"];
  if (!name) { req->send(400, "application/json", "{\"ok\":false}"); return; }
  petNameSet(name);
  req->send(200, "application/json", "{\"ok\":true}");
}

// POST /api/species
static void handleSpecies(AsyncWebServerRequest* req, JsonVariant& json) {
  uint8_t idx = json["idx"] | 0xFF;
  extern bool buddyMode, gifAvailable;
  extern void buddySetSpeciesIdx(uint8_t);
  speciesIdxSave(idx);
  buddyMode = !(gifAvailable && idx == 0xFF);
  if (buddyMode) buddySetSpeciesIdx(idx);
  req->send(200, "application/json", "{\"ok\":true}");
}

// POST /api/wifi
static void handleWifi(AsyncWebServerRequest* req, JsonVariant& json) {
  const char* action = json["action"];
  if (!action) { req->send(400, "application/json", "{\"ok\":false}"); return; }

  if (strcmp(action, "scan") == 0) {
    wifiMgrScan([req](String result) {
      AsyncWebServerResponse* resp = req->beginResponse(200, "application/json", result);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      req->send(resp);
    });
    return;
  }

  if (strcmp(action, "connect") == 0) {
    const char* ssid = json["ssid"];
    const char* pass = json["pass"];
    if (!ssid) { req->send(400, "application/json", "{\"ok\":false}"); return; }
    wifiMgrConnect(ssid, pass ? pass : "");
    req->send(200, "application/json", "{\"ok\":true}");
    return;
  }

  if (strcmp(action, "disconnect") == 0) {
    wifiMgrDisconnect();
    req->send(200, "application/json", "{\"ok\":true}");
    return;
  }

  req->send(400, "application/json", "{\"error\":\"unknown action\"}");
}

// POST /api/beep
static void handleBeep(AsyncWebServerRequest* req, JsonVariant& json) {
  uint16_t freq = json["freq"] | 440;
  uint16_t dur  = json["dur"]  | 200;
  if (dur > 5000) dur = 5000;
  hwBeep(freq, dur);
  req->send(200, "application/json", "{\"ok\":true}");
}

// ArJsonRequestHandler wrappers
static void onDisplay(AsyncWebServerRequest* req, JsonVariant& json)    { handleDisplay(req, json); }
static void onBrightness(AsyncWebServerRequest* req, JsonVariant& json) { handleBrightness(req, json); }
static void onSound(AsyncWebServerRequest* req, JsonVariant& json)      { handleSound(req, json); }
static void onName(AsyncWebServerRequest* req, JsonVariant& json)       { handleName(req, json); }
static void onSpecies(AsyncWebServerRequest* req, JsonVariant& json)    { handleSpecies(req, json); }
static void onWifi(AsyncWebServerRequest* req, JsonVariant& json)       { handleWifi(req, json); }
static void onBeep(AsyncWebServerRequest* req, JsonVariant& json)       { handleBeep(req, json); }

// ── Permission request system ─────────────────────────────────────────
struct PermReq {
  String   id;
  String   tool;
  String   hint;
  uint32_t created;
  String   result;   // ""=pending, "approved", "denied", "timeout"
};
static PermReq s_perm;
static bool    s_permActive = false;
static const uint32_t PERM_TIMEOUT_MS = 60000;

// Button hit rects (canvas coords, matching permDraw layout)
static const int PERM_BTN_X = 20, PERM_BTN_Y0 = 190, PERM_BTN_Y1 = 225;
static const int PERM_BTN_W = 200, PERM_BTN_H = 28;

// POST /api/permission
static void onPermission(AsyncWebServerRequest* req, JsonVariant& json) {
  const char* id   = json["id"];
  const char* tool = json["tool"];
  const char* hint = json["hint"];
  if (!id) {
    AsyncWebServerResponse* resp = req->beginResponse(400, "application/json",
      "{\"error\":\"missing id\"}");
    addCors(resp);
    req->send(resp);
    return;
  }
  if (s_permActive && s_perm.result.isEmpty()) {
    AsyncWebServerResponse* resp = req->beginResponse(409, "application/json",
      "{\"error\":\"busy\",\"current\":\"" + s_perm.id + "\"}");
    addCors(resp);
    req->send(resp);
    return;
  }
  s_perm.id      = id;
  s_perm.tool    = tool ? tool : "";
  s_perm.hint    = hint ? hint : "";
  s_perm.created = millis();
  s_perm.result  = "";
  s_permActive   = true;
  webLog("PERM: %s tool=%s", id, s_perm.tool.c_str());
  Serial.printf("[perm] new id=%s tool=%s hint=%s\n", id, s_perm.tool.c_str(), s_perm.hint.c_str());
  AsyncWebServerResponse* resp = req->beginResponse(202, "application/json",
    "{\"ok\":true,\"id\":\"" + s_perm.id + "\"}");
  addCors(resp);
  req->send(resp);
}

// GET /api/permission?id=xxx
static void handlePermPoll(AsyncWebServerRequest* req) {
  String id = req->arg("id");
  if (id.isEmpty()) {
    AsyncWebServerResponse* resp = req->beginResponse(400, "application/json",
      "{\"error\":\"missing id\"}");
    addCors(resp);
    req->send(resp);
    return;
  }
  if (!s_permActive || s_perm.id != id) {
    AsyncWebServerResponse* resp = req->beginResponse(404, "application/json",
      "{\"error\":\"not found\"}");
    addCors(resp);
    req->send(resp);
    return;
  }
  String r = s_perm.result.isEmpty() ? "pending" : s_perm.result;
  String json = "{\"id\":\"" + s_perm.id + "\",\"result\":\"" + r + "\"}";
  AsyncWebServerResponse* resp = req->beginResponse(200, "application/json", json);
  addCors(resp);
  req->send(resp);
}

// ── Permission display helpers ────────────────────────────────────────
static void drawBtn(int x, int y, int w, int h, const char* label, uint16_t bg, uint16_t fg) {
  Arduino_Canvas* c = hwCanvas();
  if (!c) return;
  c->fillRect(x, y, w, h, bg);
  c->drawRect(x, y, w, h, 0x7BEF);
  int tw = strlen(label) * 6;
  c->setCursor(x + (w - tw) / 2, y + (h - 8) / 2);
  c->setTextColor(fg);
  c->setTextSize(1);
  c->print(label);
}

void permDraw() {
  if (!s_permActive || !s_perm.result.isEmpty()) return;
  Arduino_Canvas* c = hwCanvas();
  if (!c) return;
  // Title bar
  c->fillRect(0, 0, HW_W, 26, 0x001F);
  c->setTextColor(WHITE);
  c->setTextSize(2);
  c->setCursor(6, 4);
  c->print("Permission");
  // Tool (text size 2)
  c->setTextColor(0xCE79);
  c->setTextSize(1);
  c->setCursor(6, 38);
  c->print("Tool: ");
  c->setTextColor(WHITE);
  c->setTextSize(2);
  c->print(s_perm.tool);
  // Hint (wrap at 36 chars for 240-wide canvas)
  c->setTextColor(0xCE79);
  c->setTextSize(1);
  c->setCursor(6, 62);
  c->print("Cmd: ");
  c->setTextColor(WHITE);
  String h = s_perm.hint;
  int wrap = 36;
  if (h.length() <= wrap) {
    c->print(h);
  } else {
    c->print(h.substring(0, wrap));
    c->setCursor(6, 74);
    c->print(h.substring(wrap, min((int)h.length(), wrap * 2)));
  }
  // Buttons — tap to confirm
  drawBtn(PERM_BTN_X, PERM_BTN_Y0, PERM_BTN_W, PERM_BTN_H, "Approve", 0x07E0, BLACK);
  drawBtn(PERM_BTN_X, PERM_BTN_Y1, PERM_BTN_W, PERM_BTN_H, "Deny",    0xC104, WHITE);
}

void permInput() {
  if (!s_permActive || !s_perm.result.isEmpty()) return;
  const HwTouch& t = hwTouch();
  if (!t.justPressed) return;

  // Hit-test: Approve button
  if (t.x >= PERM_BTN_X && t.x <= PERM_BTN_X + PERM_BTN_W &&
      t.y >= PERM_BTN_Y0 && t.y <= PERM_BTN_Y0 + PERM_BTN_H) {
    s_perm.result = "approved";
  }
  // Hit-test: Deny button
  else if (t.x >= PERM_BTN_X && t.x <= PERM_BTN_X + PERM_BTN_W &&
           t.y >= PERM_BTN_Y1 && t.y <= PERM_BTN_Y1 + PERM_BTN_H) {
    s_perm.result = "denied";
  } else {
    return; // tap outside buttons, ignore
  }

  webLog("PERM: %s -> %s", s_perm.id.c_str(), s_perm.result.c_str());
  Serial.printf("[perm] result id=%s -> %s\n", s_perm.id.c_str(), s_perm.result.c_str());
}

void permTick() {
  if (!s_permActive || !s_perm.result.isEmpty()) return;
  if (millis() - s_perm.created > PERM_TIMEOUT_MS) {
    s_perm.result = "timeout";
    webLog("PERM: %s -> timeout", s_perm.id.c_str());
    Serial.printf("[perm] timeout id=%s\n", s_perm.id.c_str());
  }
}

bool permActive() {
  return s_permActive && s_perm.result.isEmpty();
}

// Init
void webServerInit() {
  if (!LittleFS.begin(true)) {
    Serial.println("[web] LittleFS mount FAILED");
  }

  if (MDNS.begin("deskpet")) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("[web] mDNS: deskpet.local");
  } else {
    Serial.println("[web] mDNS FAILED");
  }

  // CORS preflight
  server.on("/api/*", HTTP_OPTIONS, [](AsyncWebServerRequest* req) {
    AsyncWebServerResponse* resp = req->beginResponse(204);
    addCors(resp);
    req->send(resp);
  });

  // REST endpoints
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/display",    HTTP_POST, onDisplay);
  server.on("/api/brightness", HTTP_POST, onBrightness);
  server.on("/api/sound",      HTTP_POST, onSound);
  server.on("/api/name",       HTTP_POST, onName);
  server.on("/api/species",    HTTP_POST, onSpecies);
  server.on("/api/led",        HTTP_POST, onLed);
  server.on("/api/wifi",       HTTP_POST, onWifi);
  server.on("/api/beep",       HTTP_POST, onBeep);
  server.on("/api/permission", HTTP_POST, onPermission);
  server.on("/api/permission", HTTP_GET,  handlePermPoll);

  // Log endpoint
  server.on("/api/log", HTTP_GET, [](AsyncWebServerRequest* req) {
    AsyncWebServerResponse* resp = req->beginResponse(200, "application/json", webLogGet());
    addCors(resp);
    req->send(resp);
  });

  // Static files from LittleFS /web/
  server.serveStatic("/", LittleFS, "/web/").setDefaultFile("index.html");

  // SPA fallback
  server.onNotFound([](AsyncWebServerRequest* req) {
    if (req->url().startsWith("/api/")) {
      AsyncWebServerResponse* resp = req->beginResponse(404, "application/json", "{\"error\":\"not found\"}");
      addCors(resp);
      req->send(resp);
      return;
    }
    if (LittleFS.exists("/web/index.html")) {
      AsyncWebServerResponse* resp = req->beginResponse(LittleFS, "/web/index.html", "text/html");
      addCors(resp);
      req->send(resp);
    } else {
      req->send(404, "text/plain", "404 Not Found");
    }
  });

  server.begin();
  s_webRunning = true;
  Serial.println("[web] HTTP server started on port 80");
}

bool webServerRunning() { return s_webRunning && wifiMgrConnected(); }
const char* webClawdState() { return s_clawdState.c_str(); }
