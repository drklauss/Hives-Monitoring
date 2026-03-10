#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "config.h"
#include "settings.h"

// Проверка нажатия кнопки при пробуждении
// Возвращает true если кнопка была нажата (коротко или долго)
bool checkButton()
{
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    delay(50); // Небольшая задержка для стабилизации

    if (digitalRead(PIN_BUTTON) == LOW)
    {
        LOG_W("BUTTON", "Button pressed, checking hold time...");

        unsigned long pressStart = millis();

        // Ждём либо отпускания кнопки, либо истечения времени удержания
        while (digitalRead(PIN_BUTTON) == LOW)
        {
            if (millis() - pressStart > BUTTON_HOLD_TIME)
            {
                // Удержание более BUTTON_HOLD_TIME мс = режим конфигурации
                LOG_W("BUTTON", "🔘 Long press -> Config mode");

                // Визуальная индикация (3 медленных мигания)
                for (int i = 0; i < 3; i++)
                {
                    digitalWrite(PIN_LED_BUILTIN, LOW);
                    delay(300);
                    digitalWrite(PIN_LED_BUILTIN, HIGH);
                    delay(300);
                }

                buttonMode = BUTTON_MODE_CONFIG;
                saveSettings();
                return true;
            }
            delay(50);
        }

        // Короткое нажатие = Debug режим
        LOG_W("BUTTON", "🔘 Short press -> Debug mode (%d sec)", DEBUG_MODE_DURATION);

        // Визуальная индикация (2 быстрых мигания)
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(PIN_LED_BUILTIN, LOW);
            delay(100);
            digitalWrite(PIN_LED_BUILTIN, HIGH);
            delay(100);
        }

        buttonMode = BUTTON_MODE_DEBUG;
        saveSettings();
        return true;
    }

    return false;
}

// Проверка кнопки во время работы (для перехода в режим конфигурации)
// Вызывается в цикле while в режимах OTA/Config
void checkButtonDuringOperation()
{
    if (digitalRead(PIN_BUTTON) != LOW)
    {
        return;
    }

    delay(50);
    if (digitalRead(PIN_BUTTON) == LOW)
    {
        unsigned long pressStart = millis();

        while (digitalRead(PIN_BUTTON) == LOW)
        {
            if (millis() - pressStart > BUTTON_HOLD_TIME)
            {
                LOG_W("BUTTON", "🔘 Long press during operation -> Config mode");

                // Визуальная индикация
                for (int i = 0; i < 3; i++)
                {
                    digitalWrite(PIN_LED_BUILTIN, LOW);
                    delay(300);
                    digitalWrite(PIN_LED_BUILTIN, HIGH);
                    delay(300);
                }

                buttonMode = BUTTON_MODE_CONFIG;
                saveSettings();
                ESP.restart();
            }
            delay(50);
        }
    }
}

#endif
