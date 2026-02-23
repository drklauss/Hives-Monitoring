#ifndef SYSTEM_MODES_H
#define SYSTEM_MODES_H

#include "config.h"
#include "settings.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "ota_manager.h"
#include "mqtt_manager.h"
#include "sensors.h"
#include "hive_control.h"

// ========== РЕЖИМ КОНФИГУРАЦИИ ==========
void runConfigurationMode() {
    LOG_W("MODE", "⚙️ Starting configuration mode...");
    
    wakeupReason = WAKEUP_REASON_CONFIG;
    startAPMode();
    initWebServer();
    initOTA();
    
    LOG_W("MODE", "Configuration mode active. Connect to WiFi: %s", AP_SSID);
    
    unsigned long lastBlink = 0;
    bool ledState = false;
    
    while (true) {
        handleWebServer();
        handleOTA();
        
        if (millis() - lastBlink > 1000) {
            ledState = !ledState;
            digitalWrite(PIN_LED_BUILTIN, ledState);
            lastBlink = millis();
        }
        
        if (forceOTAMode) {
            LOG_W("MODE", "New settings saved - restarting");
            delay(1000);
            ESP.restart();
        }
        
        delay(10);
    }
}

// ========== НОРМАЛЬНЫЙ РЕЖИМ ==========
void runNormalMode() {
    LOG_W("MODE", "✅ Normal operation mode");
    
    readSensors();
    
    if (connectToWiFiWithTimeout(30000)) {
        initMQTT();
        
        int attempts = 0;
        while (!mqttClient.connected() && attempts < 10) {
            handleMQTT();
            delay(500);
            attempts++;
        }
        
        if (mqttClient.connected()) {
            publishHiveData();
        } else {
            LOG_W("MODE", "MQTT connection failed");
        }
        
        unsigned long otaWindow = millis();
        while (millis() - otaWindow < OTA_WINDOW_TIME) {
            handleOTA();
            delay(10);
        }
    } else {
        LOG_W("MODE", "WiFi connection failed");
    }
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

// ========== ОБРАБОТКА ПОТЕРИ СВЯЗИ ==========
void handleConnectionLoss() {
    LOG_W("MODE", "📡 WiFi connection lost");
    
    for (int i = 0; i < 3; i++) {
        LOG_W("MODE", "Reconnection attempt %d/3", i+1);
        
        if (connectToWiFiWithTimeout(10000)) {
            LOG_W("MODE", "✅ Reconnected successfully");
            return;
        }
    }
    
    LOG_W("MODE", "❌ Failed to reconnect after 3 attempts");
    LOG_W("MODE", "😴 Deep sleep until manual reset");
    
    for (int i = 0; i < 5; i++) {
        digitalWrite(PIN_LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(PIN_LED_BUILTIN, HIGH);
        delay(100);
    }
    
    esp_deep_sleep_start();
}

#endif