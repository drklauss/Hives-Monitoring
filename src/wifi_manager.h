#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h> 
#include <DNSServer.h>
#include <WebServer.h>
#include "config.h"
#include "secrets.h"
#include "settings.h" 

extern Settings settings;
extern WebServer server;
extern DNSServer dnsServer;
extern void saveSettings();


// ========== ПОДКЛЮЧЕНИЕ С ТАЙМАУТОМ ==========
bool connectToWiFiWithTimeout(unsigned long timeoutMs) {
    if (strlen(settings.wifi_ssid) == 0) {
        LOG_W("WiFi", "No WiFi credentials saved");
        return false;
    }
    
    LOG_W("WiFi", "Connecting to %s (timeout: %d ms)", 
          settings.wifi_ssid, timeoutMs);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifi_ssid, settings.wifi_password);
    
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(100);
        digitalWrite(PIN_LED_BUILTIN, !digitalRead(PIN_LED_BUILTIN));
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        LOG_W("WiFi", "✅ Connected, IP: %s", WiFi.localIP().toString().c_str());
        digitalWrite(PIN_LED_BUILTIN, HIGH);
        return true;
    } else {
        LOG_W("WiFi", "❌ Connection timeout");
        return false;
    }
}

// ========== ЗАПУСК AP РЕЖИМА ==========
void startAPMode() {
    LOG_W("WiFi", "📡 Starting configuration AP mode...");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    
    IPAddress apIP = WiFi.softAPIP();
    LOG_W("WiFi", "✅ AP started: %s", AP_SSID);
    LOG_W("WiFi", "   Password: %s", AP_PASSWORD);
    LOG_W("WiFi", "   Admin page: http://%s", apIP.toString().c_str());
}

// ========== ПРОВЕРКА СОСТОЯНИЯ WI-FI ==========
void checkWiFiConnection() {
    static uint32_t lastCheck = 0;
    
    if (millis() - lastCheck > 30000) {
        if (WiFi.status() != WL_CONNECTED && WiFi.getMode() == WIFI_STA) {
            LOG_W("WiFi", "📡 WiFi disconnected, reconnecting...");
            WiFi.reconnect();
        }
        lastCheck = millis();
    }
}

// ========== СКАНИРОВАНИЕ СЕТЕЙ ==========
void scanWiFiNetworks() {
    LOG_W("WiFi", "🔍 Scanning networks...");
    
    int n = WiFi.scanNetworks();
    String json = "[";
    
    for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        
        uint8_t encType = WiFi.encryptionType(i);
        bool encrypted = (encType != WIFI_AUTH_OPEN);
        
        json += "\"encrypted\":" + String(encrypted ? "true" : "false");
        json += "}";
    }
    
    json += "]";
    
    WiFi.scanDelete();
    server.send(200, "application/json", json);
    LOG_W("WiFi", "Scan results sent");
}

// ========== ПОДКЛЮЧЕНИЕ К ВЫБРАННОЙ СЕТИ ==========
void connectToWiFiNetwork(String ssid, String password) {
    LOG_W("WiFi", "Connecting to new network: %s", ssid.c_str());
    
    saveWiFiCredentials(ssid, password);
    
    String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='5;url=http://esp-hive.local'/>";
    html += "<style>body{font-family:Arial;text-align:center;padding:50px;background:#f0f0f0;}</style>";
    html += "</head><body>";
    html += "<h2 style='color:#4CAF50;'>✅ Настройки сохранены!</h2>";
    html += "<p>Устройство подключается к сети <strong>" + ssid + "</strong></p>";
    html += "<p>Через 5 секунд вы будете перенаправлены...</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
    
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(3000);
    ESP.restart();
}

#endif