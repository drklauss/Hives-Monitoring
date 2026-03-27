#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "secrets.h"

#define MQTT_ENABLED 1


// Безопасный WiFi клиент для TLS
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

extern int hiveId;
extern float currentWeight;
extern float currentTemperature;
extern float currentBattery;
extern bool isLogEnabled;

// ========== ИНИЦИАЛИЗАЦИЯ MQTT ==========
void initMQTT() {
    // Настройка TLS сертификатов
    secureClient.setCACert(ca_cert);
    secureClient.setCertificate(client_cert);
    secureClient.setPrivateKey(client_key);
    
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setBufferSize(512);
    LOG_W("MQTT", "MQTT-TLS initialized with server %s:%d", MQTT_SERVER, MQTT_PORT);
}

// ========== ПОДКЛЮЧЕНИЕ К MQTT ==========
void connectMQTT() {
    static uint32_t lastTry = 0;
    const uint32_t MQTT_CONNECT_TIMEOUT = 10000;

    if (mqttClient.connected()) return;
    const uint32_t MQTT_RECONNECT_DELAY = 5000;  // 5 секунд между попытками
    if (millis() - lastTry < MQTT_RECONNECT_DELAY) return;

    lastTry = millis();

    String clientId = "Hive-";
    clientId += String(hiveId);
    clientId += "-";
    clientId += String(random(0xffff), HEX);

    LOG_W("MQTT", "Connecting to MQTT-TLS as %s...", clientId.c_str());

    bool connected = false;
    unsigned long connectStart = millis();

    while (!connected && (millis() - connectStart < MQTT_CONNECT_TIMEOUT)) {
        // Подключение без логина/пароля - аутентификация по сертификату
        connected = mqttClient.connect(clientId.c_str());
        if (!connected) delay(100);
    }

    if (connected) {
        LOG_W("MQTT", "✅ Connected to MQTT broker via TLS");
    } else {
        LOG_W("MQTT", "❌ TLS Connection failed (state: %d)", mqttClient.state());
    }
}

// ========== ПУБЛИКАЦИЯ ДАННЫХ ==========
void publishHiveData() {
    if (!mqttClient.connected()) {
        LOG_W("MQTT", "Cannot publish - not connected");
        return;
    }

    JsonDocument doc;
    doc["hive_id"] = hiveId;
    doc["weight"] = currentWeight;
    doc["temp_in"] = currentTemperature;
    doc["humidity"] = currentHumidity;  // добавляем влажность
    doc["bat"] = currentBattery;
    
    float battPercent = 0;
    if (currentBattery >= 4.0) battPercent = 100;
    else if (currentBattery <= 3.3) battPercent = 0;
    else battPercent = ((currentBattery - 3.3) / 0.7) * 100;
    
    doc["bat_pct"] = battPercent;
    doc["rssi"] = WiFi.RSSI();
    
    String payload;
    serializeJson(doc, payload);

    LOG_W("MQTT", "📤 Publishing to %s", MQTT_TOPIC_HIVE);
    
    if (mqttClient.publish(MQTT_TOPIC_HIVE, payload.c_str(), false)) {
        LOG_W("MQTT", "✅ Published successfully");
    } else {
        LOG_W("MQTT", "❌ Publish failed");
    }
    delay(100);
}

// ========== ОБРАБОТКА MQTT ==========
void handleMQTT() {
    if (!mqttClient.connected()) connectMQTT();
    if (mqttClient.connected()) mqttClient.loop();
}

#endif