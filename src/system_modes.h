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
#include "button_manager.h"

// ========== РЕЖИМ КОНФИГУРАЦИИ (AP MODE) ==========
void runConfigurationMode() {
    LOG_W("MODE", "⚙️ Starting configuration mode (AP)...");
    
    startAPMode();
    initWebServer();
    initOTA();
    
    LOG_W("MODE", "Configuration mode active. Connect to WiFi: %s", AP_SSID);
    
    unsigned long lastBlink = 0;
    bool ledState = false;
    
    while (true) {
        handleWebServer();
        handleOTA();
        checkButtonDuringOperation();  // Позволяет перейти в config mode удержанием кнопки
        
        if (millis() - lastBlink > 1000) {
            ledState = !ledState;
            digitalWrite(PIN_LED_BUILTIN, ledState);
            lastBlink = millis();
        }
        
        delay(10);
    }
}

// ========== DEBUG РЕЖИМ (WiFi + Web + OTA) ==========
void runDebugMode() {
    LOG_W("MODE", "✨ Starting Debug mode (%d seconds)...", DEBUG_MODE_DURATION);
    
    // Подключаемся к WiFi
    if (!connectToWiFiWithTimeout(30000)) {
        LOG_W("MODE", "❌ WiFi connection failed in Debug mode");
        // Если не удалось подключиться, засыпаем
        buttonMode = BUTTON_MODE_NORMAL;
        saveSettings();
        goToSleep();
        return;
    }
    
    LOG_W("MODE", "✅ WiFi connected: %s", WiFi.localIP().toString().c_str());
    
    // Инициализируем веб-сервер и OTA
    initWebServer();
    initOTA();
    
    // Читаем сенсоры для отображения на веб-странице
    readSensors();
    
    unsigned long startTime = millis();
    unsigned long lastBlink = 0;
    bool ledState = false;
    
    // Работаем в течение DEBUG_MODE_DURATION секунд
    while (millis() - startTime < DEBUG_MODE_DURATION * 1000) {
        handleWebServer();
        handleOTA();
        checkButtonDuringOperation();  // Позволяет перейти в config mode удержанием кнопки
        
        // Мигание светодиодом (быстрое в Debug режиме)
        if (millis() - lastBlink > 500) {
            ledState = !ledState;
            digitalWrite(PIN_LED_BUILTIN, ledState);
            lastBlink = millis();
        }
        
        delay(10);
    }
    
    LOG_W("MODE", "Debug mode timeout, going to sleep...");
    
    // Сбрасываем режим и засыпаем
    buttonMode = BUTTON_MODE_NORMAL;
    saveSettings();
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    
    goToSleep();
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
