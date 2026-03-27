#ifndef HIVE_CONTROL_H
#define HIVE_CONTROL_H

#include "config.h"
#include "sensors.h"

// RTC переменные

extern HX711 scale;

void initHardware() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    initSensors();
    
    // Определяем причину пробуждения (только для информации)
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        LOG_W("HIVE", "Wakeup by timer");
    } else {
        LOG_W("HIVE", "Power-on reset");
    }
}

void goToSleep() {
    LOG_W("HIVE", "😴 Sleep for %d s", sleepInterval);
    
    // Только таймер! Никакого пробуждения по GPIO
    esp_sleep_enable_timer_wakeup(sleepInterval * 1000000ULL);
    
    scale.power_down();
    
    Serial.flush();
    delay(10);
    esp_deep_sleep_start();
}

#endif