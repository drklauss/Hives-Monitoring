#ifndef HIVE_CONTROL_H
#define HIVE_CONTROL_H

#include "config.h"
#include "sensors.h"

RTC_DATA_ATTR int bootCount = 0;
extern uint8_t wakeupReason;
extern HX711 scale;
extern bool forceOTAMode;
extern void handleOTA();  // из ota_manager.h

// ========== ИНИЦИАЛИЗАЦИЯ ОБОРУДОВАНИЯ ==========
void initHardware() {
    pinMode(PIN_LED_BUILTIN, OUTPUT);
    pinMode(PIN_OTA_BUTTON, INPUT_PULLUP);
    digitalWrite(PIN_LED_BUILTIN, HIGH);
    
    initSensors();
    
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        wakeupReason = WAKEUP_REASON_TIMER;
        LOG_W("HIVE", "Wakeup by timer, boot #%d", ++bootCount);
    } else {
        wakeupReason = WAKEUP_REASON_GPIO;
        LOG_W("HIVE", "Wakeup by GPIO (reset or button)");
        bootCount = 0;
    }
}

// ========== ПРОВЕРКА КНОПКИ OTA ==========
bool checkOTAButton() {
    delay(50);
    bool pressed = (digitalRead(PIN_OTA_BUTTON) == LOW);
    if (pressed) {
        LOG_W("HIVE", "🔘 OTA button pressed");
        for (int i = 0; i < 2; i++) {
            digitalWrite(PIN_LED_BUILTIN, LOW);
            delay(100);
            digitalWrite(PIN_LED_BUILTIN, HIGH);
            delay(100);
        }
        forceOTAMode = true;
        saveSettings();
    }
    return pressed;
}

// ========== ИНДИКАЦИЯ OTA РЕЖИМА ==========
void indicateOTAMode() {
    LOG_W("HIVE", "✨ OTA mode active");
    unsigned long start = millis();
    bool ledState = LOW;

    while (millis() - start < 10000) {
        ledState = !ledState;
        digitalWrite(PIN_LED_BUILTIN, ledState);
        for (int i = 0; i < 50; i++) {
            delay(10);
            handleOTA();
        }
    }
}

// ========== ПЕРЕХОД В ГЛУБОКИЙ СОН ==========
void goToSleep() {
    LOG_W("HIVE", "😴 Going to sleep for %d seconds...", sleepInterval);
    Serial.flush();
    
    esp_sleep_enable_timer_wakeup(sleepInterval * 1000000ULL);
    scale.power_down();
    digitalWrite(PIN_LED_BUILTIN, LOW);
    esp_deep_sleep_start();
}

#endif