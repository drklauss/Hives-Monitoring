#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "config.h"
#include "settings.h"

void checkResetButton() {
    pinMode(PIN_RESET_BUTTON, INPUT_PULLUP);
    
    if (digitalRead(PIN_RESET_BUTTON) == LOW) {
        LOG_W("BUTTON", "Reset button pressed, hold for %d seconds...", RESET_HOLD_TIME/1000);
        
        unsigned long pressStart = millis();
        
        while (digitalRead(PIN_RESET_BUTTON) == LOW) {
            if (millis() - pressStart > RESET_HOLD_TIME) {
                LOG_W("BUTTON", "❗ Factory reset!");
                
                // Визуальная индикация
                for (int i = 0; i < 5; i++) {
                    digitalWrite(PIN_LED_BUILTIN, LOW);
                    delay(200);
                    digitalWrite(PIN_LED_BUILTIN, HIGH);
                    delay(200);
                }
                
                resetToDefaults();
                ESP.restart();
            }
            delay(100);
        }
    }
}

void checkOTAModeButton() {
    pinMode(PIN_OTA_BUTTON, INPUT_PULLUP);
    delay(50);
    
    if (digitalRead(PIN_OTA_BUTTON) == LOW) {
        LOG_W("BUTTON", "🔘 OTA button pressed");
        
        // Визуальное подтверждение
        for (int i = 0; i < 2; i++) {
            digitalWrite(PIN_LED_BUILTIN, LOW);
            delay(100);
            digitalWrite(PIN_LED_BUILTIN, HIGH);
            delay(100);
        }
        
        forceOTAMode = true;
        saveSettings();
    }
}

#endif