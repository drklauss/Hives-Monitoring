#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <DNSServer.h>

// Пины для ESP32-C3
#define PIN_HX711_DT 4
#define PIN_HX711_SCK 5
#define PIN_DS18B20 6
#define PIN_BATTERY 7
#define PIN_LED_BUILTIN LED_BUILTIN
#define PIN_OTA_BUTTON 9    
#define PIN_RESET_BUTTON 10

// Режимы работы
#define WAKEUP_REASON_GPIO 0
#define WAKEUP_REASON_TIMER 1
#define WAKEUP_REASON_OTA 2
#define WAKEUP_REASON_CONFIG 3

// Временные интервалы
#define DEFAULT_SLEEP_INTERVAL 3600
#define MIN_SLEEP_INTERVAL 300
#define MAX_SLEEP_INTERVAL 86400
#define OTA_WAIT_TIME 30000
#define OTA_WINDOW_TIME 15000
#define RESET_HOLD_TIME 5000

// WiFi AP режим
#define AP_SSID "ESP-Hive-Setup"
#define AP_PASSWORD "12345678"
#define DNS_PORT 53

// MQTT топики
#define MQTT_TOPIC_HIVE "dacha/hives"
#define MQTT_RECONNECT_DELAY 5000

// Значения по умолчанию
#define DEFAULT_HIVE_ID 1
#define DEFAULT_LOG_ENABLED true

// Параметры HX711
#define HX711_SCALE_FACTOR -21.6f
#define HX711_OFFSET 0.0f

// Параметры батареи
#define BATTERY_DIVIDER_RATIO 2.0f
#define ADC_MAX_VALUE 4095
#define ADC_REF_VOLTAGE 3.3f

// Версия прошивки
#define FIRMWARE_VERSION "1.0.0"

// Параметры повторных подключений
#define MAX_RECONNECT_ATTEMPTS 3
#define RECONNECT_TIMEOUT 10000
#define SLEEP_ON_FAILURE true

// Макрос логирования (единый для всего проекта)
#define LOG_W(tag, ...) \
    if (isLogEnabled) { \
        Serial.printf("[WARN] [" tag "] " __VA_ARGS__); \
        Serial.println(); \
    }

// Глобальные переменные (объявления)
extern int hiveId;
extern float currentWeight;
extern float currentTemperature;
extern float currentBattery;
extern uint32_t sleepInterval;
extern bool isLogEnabled;       
extern uint8_t wakeupReason;
extern bool forceOTAMode;

#endif