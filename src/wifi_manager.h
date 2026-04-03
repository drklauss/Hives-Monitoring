#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "config.h"
#include "secrets.h"
#include "settings.h"
#include <WebServer.h>
#include "led.h"

extern WebServer server;
extern Settings settings;

bool connectToWiFi()
{
    if (strlen(settings.wifi_ssid) == 0 || strlen(settings.wifi_password) == 0)
    {
        ledError(WIFI_NOCRED, 200);
        return false;
    }

    String hostName = "ESP-Hive-";
    hostName += String(hiveId);

    unsigned long timeoutMs = 15000;
    for (int attempt = 1; attempt <= 3; attempt++)
    {
        // Полный сброс WiFi стека
        WiFi.disconnect(true);
        delay(100);
        WiFi.mode(WIFI_OFF);
        delay(200);
        WiFi.mode(WIFI_STA);
        delay(100);
        WiFi.setTxPower(WIFI_POWER_15dBm);
        delay(200);

        WiFi.hostname(hostName);
        WiFi.begin(settings.wifi_ssid, settings.wifi_password);

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs)
        {
            ledInfo(CONN_TRY, 50);
            delay(500);
        }
        
        // Если подключились — сразу возвращаем true
        if (WiFi.status() == WL_CONNECTED)
        {
            ledSuccess(2, 150);
            return true;
        }
        
        delay(1000);
    }
    
    return false;
}

void startAPMode()
{
    WiFi.mode(WIFI_AP);
    WiFi.setTxPower(WIFI_POWER_15dBm);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    IPAddress apIP = WiFi.softAPIP();
}

void scanWiFiNetworks()
{
    int n = WiFi.scanNetworks();
    String json = "[";

    for (int i = 0; i < n; i++)
    {
        if (i > 0)
            json += ",";

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

void connectToWiFiNetwork(String ssid, String password)
{
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