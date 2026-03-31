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
        return false;
    }
    
    unsigned long pressStart = millis();
    
    // Ждем отпускания или истечения времени удержания
    while (digitalRead(PIN_BUTTON) == LOW) {
        if (millis() - pressStart > BUTTON_HOLD_TIME) {
            // Достигнуто время удержания - Config mode
            return true;
        }
        delay(10);
    }
    
    // Кнопка отпущена раньше 3 секунд - игнорируем
    return false;
}

#endif