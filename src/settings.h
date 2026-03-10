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
    uint8_t button_mode;       // BUTTON_MODE_NORMAL, BUTTON_MODE_DEBUG, BUTTON_MODE_CONFIG
    char wifi_ssid[32];
    char wifi_password[64];
    uint32_t magic;
    uint32_t version;
} settings;

const uint32_t MAGIC_NUMBER = 0xDEADBEEF;
const uint32_t SETTINGS_VERSION = 3;  // Версия 3: force_ota -> button_mode

int hiveId = DEFAULT_HIVE_ID;
float scaleFactor = HX711_SCALE_FACTOR;
float offset = HX711_OFFSET;
uint32_t sleepInterval = DEFAULT_SLEEP_INTERVAL;
bool isLogEnabled = DEFAULT_LOG_ENABLED;
uint8_t buttonMode = BUTTON_MODE_NORMAL;
uint8_t wakeupReason = WAKEUP_REASON_TIMER;
void saveSettings();
void resetSettings();

void resetToDefaults() {
    settings.hive_id = DEFAULT_HIVE_ID;
    settings.scale_factor = HX711_SCALE_FACTOR;
    settings.offset = HX711_OFFSET;
    settings.sleep_interval = DEFAULT_SLEEP_INTERVAL;
    settings.log_enabled = DEFAULT_LOG_ENABLED;
    settings.button_mode = BUTTON_MODE_NORMAL;
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
    buttonMode = BUTTON_MODE_NORMAL;
}

void resetSettings() {
    LOG_W("SETTINGS", "⚠️ Factory reset - clearing all settings");
    
    // Clear EEPROM
    for (int i = 0; i < sizeof(Settings); i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    
    // Reset to defaults
    resetToDefaults();
}

void saveSettings() {
    settings.hive_id = hiveId;
    settings.scale_factor = scaleFactor;
    settings.offset = offset;
    settings.sleep_interval = sleepInterval;
    settings.log_enabled = isLogEnabled;
    settings.button_mode = buttonMode;
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
            buttonMode = settings.button_mode;
        } else if (settings.version == 2) {
            // Миграция с версии 2 (force_ota -> button_mode)
            hiveId = settings.hive_id;
            scaleFactor = settings.scale_factor;
            offset = settings.offset;
            sleepInterval = settings.sleep_interval;
            isLogEnabled = settings.log_enabled;
            // force_ota был bool, конвертируем в button_mode
            buttonMode = BUTTON_MODE_NORMAL;
            saveSettings();
        } else {
            // Миграция с версии 1
            hiveId = settings.hive_id;
            scaleFactor = settings.scale_factor;
            offset = settings.offset;
            sleepInterval = settings.sleep_interval;
            isLogEnabled = DEFAULT_LOG_ENABLED;
            buttonMode = BUTTON_MODE_NORMAL;
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