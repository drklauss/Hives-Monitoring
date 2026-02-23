#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <ArduinoOTA.h>
#include "config.h"
#include "secrets.h"

void initOTA() {
    // Настройка OTA
    ArduinoOTA.setHostname("esp-hive");
    ArduinoOTA.setPassword(OTA_PASSWORD);
    
    // Обработчики событий OTA
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }
        LOG_W("OTA", "🔥 Start updating %s", type.c_str());
    });
    
    ArduinoOTA.onEnd([]() {
        LOG_W("OTA", "\n✅ Update complete!");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static uint8_t lastPercent = 0;
        uint8_t percent = (progress / (total / 100));
        if (percent != lastPercent && percent % 10 == 0) {
            LOG_W("OTA", "Progress: %u%%", percent);
            lastPercent = percent;
        }
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        LOG_W("OTA", "❌ Error [%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            LOG_W("OTA", "Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            LOG_W("OTA", "Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            LOG_W("OTA", "Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            LOG_W("OTA", "Receive Failed");
        } else if (error == OTA_END_ERROR) {
            LOG_W("OTA", "End Failed");
        }
    });
    
    ArduinoOTA.begin();
    LOG_W("OTA", "✅ OTA ready");
    LOG_W("OTA", "   Hostname: esp-light");
    LOG_W("OTA", "   Password: %s", OTA_PASSWORD);
}

void handleOTA() {
    ArduinoOTA.handle();
}

#endif