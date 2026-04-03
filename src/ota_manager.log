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
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static uint8_t lastPercent = 0;
        uint8_t percent = (progress / (total / 100));
        if (percent != lastPercent && percent % 10 == 0) {
            lastPercent = percent;
        }
    });
    
   
    
    ArduinoOTA.begin();
}

void handleOTA() {
    ArduinoOTA.handle();
}

#endif