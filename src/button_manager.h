#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "config.h"
#include "settings.h"

// Проверка кнопки при старте (вызывается из setup)
// Возвращает true, если нужно войти в режим конфигурации
bool checkButtonAtBoot() {
    delay(BUTTON_DEBOUNCE_TIME);
    
    // Если кнопка не нажата при старте - Normal mode
    if (digitalRead(PIN_BUTTON) != LOW) {
        LOG_W("BUTTON", "No button press at boot - Normal mode");
        return false;
    }
    
    LOG_W("BUTTON", "Button pressed at boot, checking duration...");
    unsigned long pressStart = millis();
    
    // Ждем отпускания или истечения времени удержания
    while (digitalRead(PIN_BUTTON) == LOW) {
        if (millis() - pressStart > BUTTON_HOLD_TIME) {
            // Достигнуто время удержания - Config mode
            LOG_W("BUTTON", "Button held for 3s - Config mode");
            
            return true;
        }
        delay(10);
    }
    
    // Кнопка отпущена раньше 3 секунд - игнорируем
    LOG_W("BUTTON", "Button released after %d ms - too short, Normal mode", 
          millis() - pressStart);
    return false;
}

#endif