#ifndef SENSORS_H
#define SENSORS_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>
#include "config.h"

// Глобальные переменные для датчиков
float currentWeight = 0.0f;
float currentTemperature = 0.0f;
float currentBattery = 0.0f;

// Объекты датчиков
HX711 scale;
OneWire oneWire(PIN_DS18B20);
DallasTemperature ds18b20(&oneWire);

extern float scaleFactor;
extern float offset;

void initSensors() {
    scale.begin(PIN_HX711_DT, PIN_HX711_SCK);
    scale.set_scale(scaleFactor);
    scale.set_offset(offset);
    scale.power_up();
    
    ds18b20.begin();
    ds18b20.setResolution(10);
    
    analogReadResolution(12);
}

void readSensors() {
    LOG_W("SENSORS", "Reading sensors...");
    
    // Чтение веса (усредняем несколько измерений)
    float weightSum = 0;
    int validReadings = 0;
    for (int i = 0; i < 10; i++) {
        if (scale.is_ready()) {
            weightSum += scale.get_units();
            validReadings++;
        }
        delay(50);
    }
    currentWeight = (validReadings > 0) ? weightSum / validReadings : 0;
    
    // Чтение температуры
    ds18b20.requestTemperatures();
    currentTemperature = ds18b20.getTempCByIndex(0);
    if (currentTemperature == DEVICE_DISCONNECTED_C) {
        currentTemperature = -127.0f;
    }
    
    // Чтение напряжения батареи
    int adcValue = analogRead(PIN_BATTERY);
    float voltage = (adcValue * ADC_REF_VOLTAGE) / ADC_MAX_VALUE;
    currentBattery = voltage * BATTERY_DIVIDER_RATIO;
    
    LOG_W("SENSORS", "Weight: %.1f kg, Temp: %.1f°C, Batt: %.2fV",          currentWeight, currentTemperature, currentBattery);
}

float getBatteryPercent() {
    // Приблизительная оценка для Li-ion 4.2V -> 3.3V
    if (currentBattery >= 4.0) return 100.0f;
    if (currentBattery <= 3.3) return 0.0f;
    return ((currentBattery - 3.3) / 0.9) * 100.0f;
}

#endif