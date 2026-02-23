/**
 * ESP32-C3 Hive Monitor (Deep Sleep Version)
 * Мониторинг веса, температуры и батареи улья
 */

 #include "config.h"
 #include "secrets.h"
 #include "settings.h"
 #include "sensors.h"
 #include "hive_control.h"
 #include "button_manager.h"  // Новый файл
 #include "system_modes.h"    // Новый файл
 #include "wifi_manager.h"
 #include "mqtt_manager.h"
 #include "web_server.h"
 #include "ota_manager.h"
 
 void setup() {
     Serial.begin(115200);
     delay(100);
     
     // Инициализация оборудования
     initHardware();
     
     // Загрузка настроек из EEPROM
     loadSettings();
     
     // Проверка кнопок
     checkResetButton();      // Долгое нажатие - сброс настроек
     checkOTAModeButton();    // Короткое нажатие - OTA режим
     
     // Определяем режим работы
     if (forceOTAMode || wakeupReason != WAKEUP_REASON_TIMER) {
         // Режим конфигурации (AP + captive portal)
         runConfigurationMode();
     } else {
         // Нормальный режим (сбор данных и отправка)
         runNormalMode();
         
         // Если во время работы потеряли связь
         if (WiFi.status() != WL_CONNECTED) {
             handleConnectionLoss();
         }
         
         // Спим до следующего раза
         LOG_W("MAIN", "😴 Going to sleep for %d seconds", sleepInterval);
         goToSleep();
     }
 }
 
 void loop() {
     // Не используется в deep sleep
 }