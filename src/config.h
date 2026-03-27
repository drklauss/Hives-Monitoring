#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ========== ПИНЫ ==========
#define PIN_BUTTON 0        // Кнопка входа в режим настройки (GPIO0)
#define PIN_BATTERY 1
// AHT20 использует I2C (SDA, SCL)
#define I2C_SDA 2   // GPIO2
#define I2C_SCL 3   // GPIO3
#define PIN_HX711_DT 4
#define PIN_HX711_SCK 5



// ========== ВРЕМЕННЫЕ ИНТЕРВАЛЫ ==========
#define DEFAULT_SLEEP_INTERVAL 3600      // 1 час
#define BUTTON_HOLD_TIME 3000              // 3 сек для входа в режим настройки
#define BUTTON_DEBOUNCE_TIME 50            // Антидребезг
#define CONFIG_MODE_TIMEOUT 300000         // 5 минут таймаут в режиме настройки

// ========== WI-FI AP РЕЖИМ ==========
#define AP_SSID "-ESP-Hive-Setup"
#define AP_PASSWORD "12345678"
#define DNS_PORT 53

// ========== MQTT ==========
#define MQTT_TOPIC_HIVE "dacha/hives"

// ========== ЗНАЧЕНИЯ ПО УМОЛЧАНИЮ ==========
#define DEFAULT_HIVE_ID 1

// ========== ПАРАМЕТРЫ ДАТЧИКОВ ==========
#define HX711_SCALE_FACTOR 56000.0f
#define HX711_SCALE_OFFSET -1558000.0f // вес пустого устройства с планкой
#define BAT_DIVIDER_RATIO 2.016f // полученное значение делителя напряжения
#define ADC_MAX_VALUE 4095
#define ADC_REF_VOLTAGE 3.03f // подобрал опорное

// ========== ВЕРСИЯ ==========
#define FIRMWARE_VERSION "2.0.0"

#ifndef DEBUG_MODE
#define DEBUG_MODE 0  // По умолчанию отладка выключена
#endif

// ========== МАКРОС ЛОГИРОВАНИЯ ==========
#define LOG_W(tag, ...) \
    if (isLogEnabled) { \
        Serial.printf("[WARN] [" tag "] " __VA_ARGS__); \
        Serial.println(); \
    }

// ========== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ==========
extern int hiveId;
extern float currentWeight;
extern float currentTemperature;
extern float currentHumidity;
extern float currentBattery;
extern uint32_t sleepInterval;
extern bool isLogEnabled;
extern float scaleFactor;
extern float scaleOffset;

#endif