#ifndef SENSORS_H
#define SENSORS_H

#include <HX711.h>
#include <Adafruit_AHTX0.h>
#include "config.h"

// Глобальные переменные датчиков
float currentWeight = 0.0f;
float currentTemperature = 0.0f;
float currentHumidity = 0.0f;
float currentBattery = 0.0f;

// Объекты датчиков
HX711 scale;
Adafruit_AHTX0 aht;

extern float scaleFactor;
extern float scaleOffset;

void initSensors()
{
    // Инициализация HX711
    scale.begin(PIN_HX711_DT, PIN_HX711_SCK);
    scale.set_scale(scaleFactor);
    scale.set_offset(scaleOffset);
    scale.power_up();

    // Инициализация I2C и AHT20
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!aht.begin())
    {
        LOG_W("SENSORS", "❌ AHT20 not found! Check wiring.");
        currentTemperature = -127.0f;
        currentHumidity = -1.0f;
    }
    else
    {
        LOG_W("SENSORS", "✅ AHT20 initialized");
    }

    analogReadResolution(12);
}

void readSensors()
{
    LOG_W("SENSORS", "Reading...");

    // Чтение веса
    if (scale.is_ready())
    {
        currentWeight = scale.get_units(10);
    }
    delay(50);

    // Чтение температуры и влажности с AHT20
    sensors_event_t humidity, temp;
    if (aht.getEvent(&humidity, &temp))
    {
        currentTemperature = temp.temperature;
        currentHumidity = humidity.relative_humidity;
        LOG_W("SENSORS", "AHT20: T=%.1f°C, H=%.1f%%", currentTemperature, currentHumidity);
    }
    else
    {
        LOG_W("SENSORS", "⚠️ AHT20 read error");
        // Если ошибка, оставляем предыдущие значения
    }
    delay(50);

    // Чтение напряжения батареи
    int adc = analogRead(PIN_BATTERY);
    currentBattery = (adc * ADC_REF_VOLTAGE / ADC_MAX_VALUE) * BAT_DIVIDER_RATIO;

    LOG_W("SENSORS", "W:%.1f T:%.1f H:%.1f B:%.2f", currentWeight, currentTemperature, currentHumidity, currentBattery);
}

#endif