#include "config.h"
#include "secrets.h"
#include "settings.h"
#include "sensors.h"
#include "hive_control.h"
#include "system_modes.h"
#include "button_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "web_server.h"
#include "ota_manager.h"

void setup() {
    Serial.begin(115200);
    delay(100);
    
    initHardware();
    loadSettings();
    
    // Проверяем кнопку при старте
    bool configMode = checkButtonAtBoot();
    
    if (configMode) {
        // Режим настройки + отладки
        LOG_W("MAIN", "Starting CONFIG/DEBUG mode");
        runConfigMode();
    } else {
        // Нормальный режим
        LOG_W("MAIN", "Starting NORMAL mode");
        
        // Пытаемся подключиться к WiFi
        if (connectToWiFiWithTimeout(30000)) {
            // WiFi есть - читаем датчики и отправляем
            readSensors();
            initMQTT();
            
            for (int i = 0; i < 10 && !mqttClient.connected(); i++) {
                handleMQTT();
                delay(500);
            }
            
            if (mqttClient.connected()) {
                publishHiveData();
            }
            
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
        } else {
            // WiFi нет - просто спим
            LOG_W("MAIN", "No WiFi, sleeping until next cycle");
        }
        
        goToSleep();
    }
}

void loop() {}