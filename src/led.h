#ifndef LED_H
#define LED_H

#include "config.h"

/*
 * LED ИНДИКАЦИЯ
 * ============================================================
 * 
 * ЗЕЛЕНЫЙ (ledSuccess)
 * ------------------------------------------------------------
 * START (1)          | 1 миг (400мс)  | Старт / пробуждение
 * MQTT_CON_CONNECTED (2) | 2 мига (200мс) | MQTT подключен
 * MQTT_SENT (3)      | 1 миг (200мс)  | Данные отправлены
 * 
 * СИНИЙ (ledInfo)
 * ------------------------------------------------------------
 * MQTT_CON_TRY (1)   | 1 миг (200мс)  | Попытка подключения MQTT
 * MQTT_SEND_TRY (2)  | 1 миг (100мс)  | Попытка отправки данных
 * START_CONFIG (3)   | быстрые миги   | Вход в режим настройки
 * 
 * КРАСНЫЙ (ledError)
 * ------------------------------------------------------------
 * WIFI_ERR (1)       | 1 миг (200мс)  | Ошибка WiFi
 * WIFI_NOCRED (2)    | (не используется)
 * MQTT_CON_ERR (3)   | 3 мига (300мс) | Ошибка подключения MQTT
 * MQTT_SEND_ERR (4)  | 3 мига (300мс) | Ошибка отправки данных
 * 
 * ============================================================
 * ПОЛНЫЙ ЦИКЛ УСПЕШНОЙ РАБОТЫ:
 * ------------------------------------------------------------
 * [ЗЕЛ START] -> [СИН MQTT_CON_TRY] -> [ЗЕЛ MQTT_CON_CONNECTED] -> 
 * [СИН MQTT_SEND_TRY] -> [ЗЕЛ MQTT_SENT]
 * 
 * ============================================================
 * БЫСТРАЯ ДИАГНОСТИКА:
 * ------------------------------------------------------------
 * 1x зеленый (400мс)  -> старт OK
 * 1x синий (200мс)    -> попытка MQTT
 * 2x зеленый (200мс)  -> MQTT готов
 * 1x синий (100мс)    -> отправка
 * 1x зеленый (200мс)  -> данные отправлены
 * 1x красный (200мс)  -> ошибка WiFi
 * 3x красный (300мс)  -> ошибка MQTT
 * ============================================================
 */

#define PIN_LED_RED    6
#define PIN_LED_GREEN  7
#define PIN_LED_BLUE  9




enum LedColor {
    LED_OFF,
    LED_RED,
    LED_GREEN,
    LED_BLUE,
};

enum LedSuccesState {
    UNDEF_0,
    START,
    MQTT_CON_CONNECTED,
    MQTT_SENT
};

enum LedInfoState {
    UNDEF_1,
    MQTT_CON_TRY,
    MQTT_SEND_TRY,
    START_CONFIG
};

enum LedErrorState {
    UNDEF_2,
    WIFI_ERR,
    WIFI_NOCRED, 
    MQTT_CON_ERR,
    MQTT_SEND_ERR
};



void initLED() {
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    // Для общего анода: HIGH = выключен, LOW = включен
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_LED_BLUE, HIGH);
}

void ledSetColor(LedColor color) {
    switch(color) {
        case LED_OFF:
            digitalWrite(PIN_LED_RED, HIGH);
            digitalWrite(PIN_LED_GREEN, HIGH);
            digitalWrite(PIN_LED_BLUE, HIGH);
            break;
        case LED_RED:
            digitalWrite(PIN_LED_RED, LOW);   // LOW = зажигаем красный
            digitalWrite(PIN_LED_GREEN, HIGH);
            digitalWrite(PIN_LED_BLUE, HIGH);
            break;
        case LED_GREEN:
        digitalWrite(PIN_LED_GREEN, LOW); // LOW = зажигаем зелёный
            digitalWrite(PIN_LED_RED, HIGH);
            digitalWrite(PIN_LED_BLUE, HIGH);
            break;
        case LED_BLUE:
        digitalWrite(PIN_LED_BLUE, LOW); // LOW = зажигаем синий
            digitalWrite(PIN_LED_RED, HIGH);
            digitalWrite(PIN_LED_GREEN, HIGH);
            break;
    }
}

void ledBlink(LedColor color, int count = 1, int duration = LED_BLINK_DURATION) {
    for (int i = 0; i < count; i++) {
        ledSetColor(color);
        delay(duration);
        ledSetColor(LED_OFF);
        if (i < count - 1) delay(duration);
    }
    delay(500);
}

void ledSuccess(int count = 1, int duration = LED_BLINK_DURATION) {
    ledBlink(LED_GREEN, count, duration);
}

void ledInfo(int count = 1, int duration = LED_BLINK_DURATION) {
    ledBlink(LED_BLUE, count, duration);
}

void ledError(int count = 1, int duration = LED_BLINK_DURATION) {
    ledBlink(LED_RED, count, duration);
}

#endif