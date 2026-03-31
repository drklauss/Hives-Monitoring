#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "secrets.h"
#include "led.h"

#define MQTT_MAX_RETRIES 3
#define MQTT_RETRY_DELAY 1000
#define MQTT_PUBLISH_RETRIES 2
#define MQTT_CONNECT_TIMEOUT 10000

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
    
    secureClient.setTimeout(10);          
    secureClient.setHandshakeTimeout(10);
    
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setBufferSize(512);
    mqttClient.setKeepAlive(30);
}

// ========== ПОДКЛЮЧЕНИЕ К MQTT ==========
bool connectMQTT() {
    if (mqttClient.connected()) return true;
    
    String clientId = "Hive-";
    clientId += String(hiveId);

    ledInfo(MQTT_CON_TRY, 200);
    
    unsigned long start = millis();
    while (!mqttClient.connected() && (millis() - start) < MQTT_CONNECT_TIMEOUT) {
        if (mqttClient.connect(clientId.c_str())) {
            ledSuccess(MQTT_CON_CONNECTED, 200); 
            return true;
        }
        delay(500);
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
        delay(300);
        mqttClient.loop();
    }
    
    ledError(MQTT_SEND_ERR, 300);

    return false;
}

// ========== ОТПРАВКА С ПОЛНЫМ ЦИКЛОМ ==========
bool sendMQTTData() {
    initMQTT();
    
    if (!connectMQTT()) return false;
    
    delay(100);  
    mqttClient.loop();
    
    bool result = publishHiveData();
    
    mqttClient.loop();
    mqttClient.disconnect();
    
    return result;
}

#endif