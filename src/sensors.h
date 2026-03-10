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
    
    // Чтение температуры (медианный фильтр для 3 измерений)
    float tempValues[3];
    int tempReadings = 0;
    
    for (int i = 0; i < 3; i++) {
        ds18b20.requestTemperatures();
        float temp = ds18b20.getTempCByIndex(0);
        if (temp != DEVICE_DISCONNECTED_C && temp > -50.0f && temp < 100.0f) {
            tempValues[tempReadings++] = temp;
        }
        delay(50);
    }
    
    if (tempReadings == 3) {
        // Медианный фильтр: берём среднее из трёх значений
        if (tempValues[0] > tempValues[1]) { float t = tempValues[0]; tempValues[0] = tempValues[1]; tempValues[1] = t; }
        if (tempValues[1] > tempValues[2]) { float t = tempValues[1]; tempValues[1] = tempValues[2]; tempValues[2] = t; }
        if (tempValues[0] > tempValues[1]) { float t = tempValues[0]; tempValues[0] = tempValues[1]; tempValues[1] = t; }
        currentTemperature = tempValues[1];  // Медиана
    } else if (tempReadings > 0) {
        // Если не все измерения успешны, берём среднее из того что есть
        float sum = 0;
        for (int i = 0; i < tempReadings; i++) sum += tempValues[i];
        currentTemperature = sum / tempReadings;
    } else {
        currentTemperature = -127.0f;  // Ошибка датчика
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