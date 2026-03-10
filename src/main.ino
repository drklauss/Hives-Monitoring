/**
 * ESP32-C3 Hive Monitor (Deep Sleep Version)
 * Мониторинг веса, температуры и батареи улья
 */

 #include "config.h"
 #include "secrets.h"
 #include "settings.h"
 #include "sensors.h"
 #include "hive_control.h"
 #include "button_manager.h"
 #include "system_modes.h"
 #include "wifi_manager.h"
 #include "mqtt_manager.h"
 #include "web_server.h"
 #include "ota_manager.h"
 
 void setup() {
     Serial.begin(115200);
     delay(100);
     
     // Инициализация оборудования (определяет причину пробуждения)
     initHardware();
     
     // Загрузка настроек из EEPROM
     loadSettings();
     
     // Если пробуждение по кнопке (GPIO), проверяем тип нажатия
     if (wakeupReason == WAKEUP_REASON_GPIO) {
         checkButton();  // Определяет: короткое = Debug, длинное = Config
     }
     
     // Определяем режим работы на основе buttonMode
     if (buttonMode == BUTTON_MODE_CONFIG) {
         // Режим конфигурации (AP + captive portal)
         runConfigurationMode();
     } else if (buttonMode == BUTTON_MODE_DEBUG) {
         // Debug режим (подключение к WiFi + веб-интерфейс + OTA)
         runDebugMode();
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
