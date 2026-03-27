#ifndef SYSTEM_MODES_H
#define SYSTEM_MODES_H

#include "config.h"
#include "settings.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "ota_manager.h"
#include "sensors.h"
#include "hive_control.h"

// Глобальная переменная для отслеживания активности
unsigned long lastActivity = 0;

void runConfigMode() {
    LOG_W("MODE", "⚙️ Starting config/debug mode");
    
    // Читаем датчики сразу для отображения
    readSensors();
    
    // Запускаем AP режим
    startAPMode();
    
    // Инициализируем веб-сервер и OTA
    initWebServer();
    initOTA();
    
    // Запоминаем время старта
    lastActivity = millis();
    
    LOG_W("MODE", "Connect to WiFi: %s", AP_SSID);
    LOG_W("MODE", "Open http://%s in browser", WiFi.softAPIP().toString().c_str());
    LOG_W("MODE", "Will sleep after %d seconds of inactivity", CONFIG_MODE_TIMEOUT / 1000);
    
    // Основной цикл
    while (true) {
        handleWebServer();
        handleOTA();
        
        // Проверяем таймаут бездействия
        if (millis() - lastActivity > CONFIG_MODE_TIMEOUT) {
            LOG_W("MODE", "⏰ Inactivity timeout - going to sleep");
            
            goToSleep();
        }
        
        delay(10);
    }
}

// Функция для обновления активности (вызывается из web_server.h)
void updateActivity() {
    lastActivity = millis();
}

#endif