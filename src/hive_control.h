#ifndef HIVE_CONTROL_H
#define HIVE_CONTROL_H

#include "config.h"
#include "sensors.h"

RTC_DATA_ATTR int bootCount = 0;
extern uint8_t wakeupReason;
extern uint8_t buttonMode;
extern HX711 scale;
extern void handleOTA();  // из ota_manager.h

// ========== ИНИЦИАЛИЗАЦИЯ ОБОРУДОВАНИЯ ==========
void initHardware() {
    pinMode(PIN_LED_BUILTIN, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    digitalWrite(PIN_LED_BUILTIN, HIGH);
    
    initSensors();
    
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        wakeupReason = WAKEUP_REASON_TIMER;
        LOG_W("HIVE", "Wakeup by timer, boot #%d", ++bootCount);
    } else if (cause == ESP_SLEEP_WAKEUP_GPIO) {
        // ESP32-C3 использует GPIO wakeup
        // Пробуждение по кнопке - нужно проверить длительность нажатия
        wakeupReason = WAKEUP_REASON_GPIO;
        LOG_W("HIVE", "Wakeup by button (GPIO %d)", PIN_BUTTON);
        bootCount = 0;
        // Не устанавливаем buttonMode здесь - это делается в checkButton()
    } else {
        wakeupReason = WAKEUP_REASON_GPIO;
        LOG_W("HIVE", "Wakeup by reset/power-on");
        bootCount = 0;
    }
}

// ========== ИНДИКАЦИЯ DEBUG РЕЖИМА ==========
void indicateDebugMode() {
    LOG_W("HIVE", "✨ Debug mode active for %d seconds", DEBUG_MODE_DURATION);
    unsigned long start = millis();
    bool ledState = LOW;

    while (millis() - start < DEBUG_MODE_DURATION * 1000) {
        ledState = !ledState;
        digitalWrite(PIN_LED_BUILTIN, ledState);
        for (int i = 0; i < 50; i++) {
            delay(10);
            handleOTA();
        }
    }
    
    // После истечения времени Debug режима - сбрасываем в normal и засыпаем
    LOG_W("HIVE", "Debug mode timeout, going to sleep...");
    buttonMode = BUTTON_MODE_NORMAL;
    saveSettings();
}

// ========== ПЕРЕХОД В ГЛУБОКИЙ СОН ==========
void goToSleep() {
    LOG_W("HIVE", "😴 Going to sleep for %d seconds...", sleepInterval);
    Serial.flush();
    
    // Настройка пробуждения по таймеру
    esp_sleep_enable_timer_wakeup(sleepInterval * 1000000ULL);
    
    // Настройка пробуждения по кнопке (GPIO, активный уровень LOW)
    // Для ESP32-C3 используется esp_deep_sleep_enable_gpio_wakeup
    esp_deep_sleep_enable_gpio_wakeup(1ULL << PIN_BUTTON, ESP_GPIO_WAKEUP_GPIO_LOW);
    
    scale.power_down();
    digitalWrite(PIN_LED_BUILTIN, LOW);
    esp_deep_sleep_start();
}

#endif
