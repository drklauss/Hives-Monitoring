#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "config.h"
#include "secrets.h"
#include "settings.h"
#include <DNSServer.h>
#include <WebServer.h>

extern WebServer server;
extern Settings settings;

bool connectToWiFiWithTimeout(unsigned long timeoutMs) {
    if (strlen(settings.wifi_ssid) == 0) {
        LOG_W("WiFi", "No WiFi credentials saved");
        return false;
    }
    
    LOG_W("WiFi", "Connecting to %s", settings.wifi_ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifi_ssid, settings.wifi_password);
    
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(100);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        LOG_W("WiFi", "✅ Connected, IP: %s", WiFi.localIP().toString().c_str());
        return true;
    } else {
        LOG_W("WiFi", "❌ Connection timeout");
        return false;
    }
}

void startAPMode() {
    LOG_W("WiFi", "📡 Starting AP mode...");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID+hiveId, AP_PASSWORD);
    
    IPAddress apIP = WiFi.softAPIP();
    LOG_W("WiFi", "✅ AP started: %s", AP_SSID);
    LOG_W("WiFi", "   Password: %s", AP_PASSWORD);
    LOG_W("WiFi", "   Admin page: http://%s", apIP.toString().c_str());
}

void scanWiFiNetworks() {
    LOG_W("WiFi", "🔍 Scanning networks...");
    
    int n = WiFi.scanNetworks();
    String json = "[";
    
    for (int i = 0; i < n; i++) {
        if (i > 0) json += ",";
        
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        
        bool encrypted = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        json += "\"encrypted\":" + String(encrypted ? "true" : "false");
        json += "}";
    }
    
    json += "]";
    
    WiFi.scanDelete();
    server.send(200, "application/json", json);
}

void connectToWiFiNetwork(String ssid, String password) {
    LOG_W("WiFi", "Connecting to new network: %s", ssid.c_str());
    
    saveWiFiCredentials(ssid, password);
    
    String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='5'/>";
    html += "<style>body{font-family:Arial;text-align:center;padding:50px;}</style>";
    html += "</head><body>";
    html += "<h2 style='color:#4CAF50;'>✅ Настройки сохранены!</h2>";
    html += "<p>Устройство перезагружается...</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
    
    delay(1000);
    ESP.restart();
}

#endif