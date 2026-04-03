#include "config.h"
#include "secrets.h"
#include "settings.h"
#include "sensors.h"
#include "hive_control.h"
#include "system_modes.h"
#include "button_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "web_server.h"
#include "led.h"

void setup()
{
    isLogEnabled = DEBUG_MODE;

    initHardware();
    loadSettings();

    // Проверяем кнопку при старте
    if (checkButtonAtBoot())
    {
        runConfigMode(); // Режим настройки + отладки
    }
    else
    {
        ledSuccess(START, 400);

        if (connectToWiFi())
        {
            readSensors();  // Читаем датчики
            sendMQTTData(); // Отправляем MQTT с полным циклом
            delay(500);     // Короткая пауза перед отключением
        }
        else
        {
            // WiFi нет
            ledError(WIFI_ERR, 200);
            delay(500);
        }

        goToSleep();
    }
}

void loop() {}