#ifndef SYSTEM_MODES_H
#define SYSTEM_MODES_H

#include "config.h"
#include "settings.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "ota_manager.h"
#include "sensors.h"
#include "hive_control.h"
#include "led.h"

// Глобальная переменная для отслеживания активности
unsigned long lastActivity = 0;

void runConfigMode() {
    // Быстрые мигания для индикации режима конфигурации
   ledInfo(START_CONFIG, 100);
    
    // Читаем датчики сразу для отображения
    readSensors();
    
    // Запускаем AP режим
    startAPMode();
    
    // Инициализируем веб-сервер и OTA
    initWebServer();
    initOTA();
    
    // Запоминаем время старта
    lastActivity = millis();
    
    // Основной цикл
    while (true) {
        handleWebServer();
        // handleOTA();
        
        // Проверяем таймаут бездействия
        if (millis() - lastActivity > CONFIG_MODE_TIMEOUT) {
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
