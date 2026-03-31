#ifndef HIVE_CONTROL_H
#define HIVE_CONTROL_H

#include <WiFi.h>
#include "config.h"
#include "sensors.h"
#include "led.h"

extern HX711 scale;


void initHardware() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    initLED();
    initSensors();
}

void goToSleep() {
    esp_sleep_enable_timer_wakeup(sleepInterval * 1000000ULL);
    
    scale.power_down();
    
    // Важно: отключаем подтяжку на GPIO0 (BOOT) перед сном
    // GPIO0 используется для выбора режима загрузки
    // Если оставить INPUT_PULLUP, это может помешать загрузке после пробуждения
    pinMode(PIN_BUTTON, INPUT);
    digitalWrite(PIN_BUTTON, HIGH);  // Отпускаем подтяжку
    
    // Отключаем WiFi полностью
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    
    Serial.flush();
    delay(100);  // Даем время на завершение всех операций
    
    esp_deep_sleep_start();
}

#endif