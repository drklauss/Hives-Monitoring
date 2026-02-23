#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>
#include "config.h"
#include "secrets.h"

struct Settings {
    int hive_id;
    float scale_factor;
    float offset;
    uint32_t sleep_interval;
    bool log_enabled;          
    bool force_ota;
    char wifi_ssid[32];
    char wifi_password[64];
    uint32_t magic;
    uint32_t version;
} settings;

const uint32_t MAGIC_NUMBER = 0xDEADBEEF;
const uint32_t SETTINGS_VERSION = 2;

int hiveId = DEFAULT_HIVE_ID;
float scaleFactor = HX711_SCALE_FACTOR;
float offset = HX711_OFFSET;
uint32_t sleepInterval = DEFAULT_SLEEP_INTERVAL;
bool isLogEnabled = DEFAULT_LOG_ENABLED; 
bool forceOTAMode = false;
uint8_t wakeupReason = WAKEUP_REASON_TIMER;
void saveSettings();

void resetToDefaults() {
    settings.hive_id = DEFAULT_HIVE_ID;
    settings.scale_factor = HX711_SCALE_FACTOR;
    settings.offset = HX711_OFFSET;
    settings.sleep_interval = DEFAULT_SLEEP_INTERVAL;
    settings.log_enabled = DEFAULT_LOG_ENABLED; 
    settings.force_ota = false;
    strcpy(settings.wifi_ssid, WIFI_SSID);
    strcpy(settings.wifi_password, WIFI_PASSWORD);
    settings.magic = MAGIC_NUMBER;
    settings.version = SETTINGS_VERSION;
    
    EEPROM.put(0, settings);
    EEPROM.commit();
    
    hiveId = DEFAULT_HIVE_ID;
    scaleFactor = HX711_SCALE_FACTOR;
    offset = HX711_OFFSET;
    sleepInterval = DEFAULT_SLEEP_INTERVAL;
    isLogEnabled = DEFAULT_LOG_ENABLED; 
    forceOTAMode = false;
}

void saveSettings() {
    settings.hive_id = hiveId;
    settings.scale_factor = scaleFactor;
    settings.offset = offset;
    settings.sleep_interval = sleepInterval;
    settings.log_enabled = isLogEnabled;  
    settings.force_ota = forceOTAMode;
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
            offset = settings.offset;
            sleepInterval = settings.sleep_interval;
            isLogEnabled = settings.log_enabled;  
            forceOTAMode = settings.force_ota;
        } else {
            // Миграция с версии 1
            hiveId = settings.hive_id;
            scaleFactor = settings.scale_factor;
            offset = settings.offset;
            sleepInterval = settings.sleep_interval;
            isLogEnabled = DEFAULT_LOG_ENABLED;  
            forceOTAMode = false;
            saveSettings();
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