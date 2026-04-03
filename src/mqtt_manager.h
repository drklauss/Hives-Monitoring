#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "secrets.h"
#include "led.h"

#define MQTT_PUBLISH_RETRIES 5
#define MQTT_CONNECT_TIMEOUT 30000

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

extern int hiveId;
extern float currentWeight;
extern float currentTemperature;
extern float currentHumidity;
extern float currentBattery;

// ========== ИНИЦИАЛИЗАЦИЯ MQTT ==========
void initMQTT() {
    secureClient.setCACert(ca_cert);
    secureClient.setCertificate(client_cert);
    secureClient.setPrivateKey(client_key);
    
    secureClient.setTimeout(60);          
    secureClient.setHandshakeTimeout(60);
    
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setBufferSize(512);
    mqttClient.setKeepAlive(60);
}

// ========== ПОДКЛЮЧЕНИЕ К MQTT ==========
bool connectMQTT() {
    if (mqttClient.connected()) return true;
    
    String clientId = "Hive-";
    clientId += String(hiveId);

    unsigned long start = millis();
    while (!mqttClient.connected() && (millis() - start) < MQTT_CONNECT_TIMEOUT) {
        ledInfo(CONN_TRY, 50);  // короткое мигание
        if (mqttClient.connect(clientId.c_str())) {
            mqttClient.loop();
            ledSuccess(MQTT_CON_CONNECTED, 100); 
            return true;
        }
        mqttClient.loop();
        delay(200); 
    }
    
    ledError(MQTT_CON_ERR, 300);
    return false;
}

// ========== ПУБЛИКАЦИЯ ДАННЫХ ==========
bool publishHiveData() {
    if (!mqttClient.connected()) {
        if (!connectMQTT()) return false;
    }
    
    mqttClient.loop();
    
    JsonDocument doc;
    doc["hive_id"] = hiveId;
    doc["weight"] = currentWeight;
    doc["temp_in"] = currentTemperature;
    doc["humidity"] = currentHumidity;
    doc["bat"] = currentBattery;
    doc["rssi"] = WiFi.RSSI();
    
    String payload;
    serializeJson(doc, payload);
    
    ledInfo(MQTT_SEND_TRY, 100); 
    
    // Публикация с повторными попытками
    for (int attempt = 1; attempt <= MQTT_PUBLISH_RETRIES; attempt++) {
        if (mqttClient.publish(MQTT_TOPIC_HIVE, payload.c_str(), true)) {
            ledSuccess(MQTT_SENT, 200); 

            return true;
        }

        delay(50);
        mqttClient.loop();
    }
    
    ledError(MQTT_SEND_ERR, 300);

    return false;
}

// ========== ОТПРАВКА С ПОЛНЫМ ЦИКЛОМ ==========
bool sendMQTTData() {
    initMQTT();
    
    if (!connectMQTT()) return false;
    
    for (int i = 0; i < 10; i++) {
        mqttClient.loop();
        delay(50);
    }
    
    bool result = publishHiveData();
    
    for (int i = 0; i < 5; i++) {
        mqttClient.loop();
        delay(50);
    }
    
    mqttClient.disconnect();
    delay(50);
    
    return result;
}

#endif