#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"
#include "sensors.h"
#include "wifi_manager.h"
#include "settings.h"
#include "web/html.h"
#include "web/css.h"
#include "web/js.h"

WebServer server(80);
DNSServer dnsServer;

extern void updateActivity();
extern void saveHiveSettings();
extern void saveWiFiCredentials(const String& ssid, const String& password);
extern void readSensors();
extern int hiveId;
extern uint32_t sleepInterval;
extern float scaleFactor;
extern float scaleOffset;
extern float currentWeight, currentTemperature, currentHumidity, currentBattery;
extern HX711 scale;

// ========== ОБРАБОТЧИКИ СТАТИКИ ==========
void handleCSS() {
    updateActivity();
    server.send(200, "text/css", FPSTR(STYLE_CSS));
}

void handleJS() {
    updateActivity();
    server.send(200, "application/javascript", FPSTR(SCRIPT_JS));
}

// ========== ОСНОВНАЯ СТРАНИЦА ==========
void handleRoot() {
    updateActivity();
    
    String html = FPSTR(INDEX_HTML);
    html.replace("{HIVE_ID}", String(hiveId));
    html.replace("{WEIGHT}", String(currentWeight, 2));
    html.replace("{TEMP}", String(currentTemperature, 1));
    html.replace("{HUM}", String(currentHumidity, 0));
    html.replace("{BATT}", String(currentBattery, 2));
    html.replace("{SLEEP}", String(sleepInterval));
    
    server.send(200, "text/html", html);
}

// ========== API ДЛЯ ДАТЧИКОВ ==========
void handleSensors() {
    updateActivity();
    readSensors();
    
    String json = "{";
    json += "\"weight\":" + String(currentWeight, 2) + ",";
    json += "\"temp\":" + String(currentTemperature, 1) + ",";
    json += "\"humidity\":" + String(currentHumidity, 0) + ",";
    json += "\"battery\":" + String(currentBattery, 2);
    json += "}";
    
    server.send(200, "application/json", json);
}

// ========== API ДЛЯ КАЛИБРОВКИ (получение) ==========
void handleGetCalibration() {
    updateActivity();
    
    long raw = scale.read();
    
    String json = "{";
    json += "\"raw\":" + String(raw) + ",";
    json += "\"offset\":" + String(scaleOffset, 0) + ",";
    json += "\"factor\":" + String(scaleFactor, 1);
    json += "}";
    
    server.send(200, "application/json", json);
}

// ========== КАЛИБРОВКА: установка offset (тара) ==========
void handleSetOffset() {
    updateActivity();
    
    if (!scale.is_ready()) {
        server.send(500, "text/plain", "HX711 not ready");
        return;
    }
    
    scaleOffset = scale.read();
    scale.set_offset(scaleOffset);
    saveHiveSettings();
    
    server.send(200, "text/plain", "✅ Offset saved: " + String(scaleOffset, 0));
}

// ========== КАЛИБРОВКА: установка factor (коэффициент) ==========
void handleSetFactor() {
    updateActivity();
    
    if (!server.hasArg("weight")) {
        server.send(400, "text/plain", "Missing weight");
        return;
    }
    
    float knownWeight = server.arg("weight").toFloat();
    if (knownWeight <= 0) {
        server.send(400, "text/plain", "Invalid weight");
        return;
    }
    
    if (!scale.is_ready()) {
        server.send(500, "text/plain", "HX711 not ready");
        return;
    }
    
    long currentRaw = scale.read();
    long diff = currentRaw - scaleOffset;
    
    if (diff == 0) {
        server.send(400, "text/plain", "No change from offset. Did you set offset first?");
        return;
    }
    
    scaleFactor = diff / knownWeight;
    scale.set_scale(scaleFactor);
    saveHiveSettings();
    
    server.send(200, "text/plain", 
        "✅ Factor saved: " + String(scaleFactor, 1) + "\nOffset: " + String(scaleOffset, 0));
}

// ========== НАСТРОЙКИ УЛЬЯ ==========
void handleSaveHive() {
    updateActivity();
    
    if (server.hasArg("hive_id")) {
        hiveId = server.arg("hive_id").toInt();
    }
    if (server.hasArg("sleep_interval")) {
        sleepInterval = server.arg("sleep_interval").toInt();
    }
    
    saveHiveSettings();
    server.send(200, "text/plain", "✅ Hive settings saved");
}

// ========== WIFI ==========
void handleWiFiScan() {
    updateActivity();
    scanWiFiNetworks();
}

void handleWiFiConnect() {
    updateActivity();
    
    if (!server.hasArg("ssid")) {
        server.send(400, "text/plain", "Missing ssid");
        return;
    }
    
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    saveWiFiCredentials(ssid, password);
    
    String html = "<html><body style='font-family:system-ui;text-align:center;padding:2rem;'>";
    html += "<h1>✅ WiFi saved!</h1>";
    html += "<p>Device restarting...</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
    delay(1000);
    ESP.restart();
}

void handleCaptivePortal() {
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString(), true);
    server.send(302, "text/plain", "");
}

// ========== ИНИЦИАЛИЗАЦИЯ ==========
void initWebServer() {
    server.on("/", handleRoot);
    server.on("/style.css", handleCSS);
    server.on("/script.js", handleJS);
    server.on("/sensors", handleSensors);
    server.on("/getCalibration", handleGetCalibration);
    server.on("/setOffset", HTTP_POST, handleSetOffset);
    server.on("/setFactor", HTTP_POST, handleSetFactor);
    server.on("/saveHive", HTTP_POST, handleSaveHive);
    server.on("/scan", handleWiFiScan);
    server.on("/connect", HTTP_POST, handleWiFiConnect);
    server.onNotFound(handleCaptivePortal);
    
    server.begin();
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
}

void handleWebServer() {
    server.handleClient();
    dnsServer.processNextRequest();
}

#endif