#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include "config.h"
#include "secrets.h"

struct Settings {
    int hive_id;
    float scale_factor;
    float scale_offset;
    uint32_t sleep_interval;
    char wifi_ssid[32];
    char wifi_password[64];
    uint32_t magic;
    uint32_t version;
} settings;

const uint32_t MAGIC_NUMBER = 0xDEADBEEF;
const uint32_t SETTINGS_VERSION = 1;

// Глобальные переменные (инициализация)
int hiveId = DEFAULT_HIVE_ID;
float scaleFactor = HX711_SCALE_FACTOR;
float scaleOffset = HX711_SCALE_OFFSET;
uint32_t sleepInterval = DEFAULT_SLEEP_INTERVAL;
bool isLogEnabled = DEBUG_MODE;

// Прототипы
void saveHiveSettings();
void resetSettings();

void resetToDefaults() {
    settings.hive_id = DEFAULT_HIVE_ID;
    settings.scale_factor = HX711_SCALE_FACTOR;
    settings.scale_offset = HX711_SCALE_OFFSET;
    settings.sleep_interval = DEFAULT_SLEEP_INTERVAL;
    strcpy(settings.wifi_ssid, WIFI_SSID);
    strcpy(settings.wifi_password, WIFI_PASSWORD);
    settings.magic = MAGIC_NUMBER;
    settings.version = SETTINGS_VERSION;
    
    EEPROM.put(0, settings);
    EEPROM.commit();
    
    hiveId = DEFAULT_HIVE_ID;
    scaleFactor = HX711_SCALE_FACTOR;
    scaleOffset = HX711_SCALE_OFFSET;
    sleepInterval = DEFAULT_SLEEP_INTERVAL;
    isLogEnabled = DEBUG_MODE;
}

void resetSettings() {
    LOG_W("SETTINGS", "⚠️ Factory reset");
    for (int i = 0; i < sizeof(Settings); i++) EEPROM.write(i, 0);
    EEPROM.commit();
    resetToDefaults();
}

void saveHiveSettings() {
    settings.hive_id = hiveId;
    settings.scale_factor = scaleFactor;
    settings.sleep_interval = sleepInterval;
    settings.magic = MAGIC_NUMBER;
    settings.version = SETTINGS_VERSION;
    
    EEPROM.put(0, settings);
    EEPROM.commit();
}

void loadSettings() {
    EEPROM.begin(sizeof(Settings));
    EEPROM.get(0, settings);
    
    if (settings.magic == MAGIC_NUMBER) {
        if (settings.version == SETTINGS_VERSION) {
            hiveId = settings.hive_id;
            scaleFactor = settings.scale_factor;
            scaleOffset = settings.scale_offset;  
            sleepInterval = settings.sleep_interval;
        } else {
            hiveId = settings.hive_id;
            scaleFactor = settings.scale_factor;
            scaleOffset = settings.scale_offset;
            sleepInterval = settings.sleep_interval;
            saveHiveSettings();
        }
    } else {
        resetToDefaults();
    }
}

void saveWiFiCredentials(const String& ssid, const String& password) {
    strlcpy(settings.wifi_ssid, ssid.c_str(), sizeof(settings.wifi_ssid));
    strlcpy(settings.wifi_password, password.c_str(), sizeof(settings.wifi_password));
    settings.magic = MAGIC_NUMBER;
    settings.version = SETTINGS_VERSION;
    EEPROM.put(0, settings);
    EEPROM.commit();
}

#endif