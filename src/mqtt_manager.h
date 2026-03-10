#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "secrets.h"

#define MQTT_ENABLED 1

// ========== СЕРТИФИКАТЫ TLS ==========
// Корневой сертификат CA
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFhzCCA2+gAwIBAgIUUHqPBsjYy4y9PtGAKq2ANkbSOhswDQYJKoZIhvcNAQEL
BQAwUzELMAkGA1UEBhMCUlUxDzANBgNVBAgMBk1vc2NvdzEPMA0GA1UEBwwGTW9z
Y293MQ8wDQYDVQQKDAZMZW52ZXIxETAPBgNVBAMMCExlbnZlckNBMB4XDTI2MDIy
NjA4NTQwMVoXDTM2MDIyNDA4NTQwMVowUzELMAkGA1UEBhMCUlUxDzANBgNVBAgM
Bk1vc2NvdzEPMA0GA1UEBwwGTW9zY293MQ8wDQYDVQQKDAZMZW52ZXIxETAPBgNV
BAMMCExlbnZlckNBMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAzjZk
459rFQuZViIPPKPwMnIjscHCp6Tz0GusMs0mQMZ12Y3fAOOqUIRzZr07BLz8odNq
NN3raE275xws8SR2tt00TsKRYlC37K9uc9GNmmQNGCVbJmwnBSx9E8ltUjmtcdQg
TBb/3PH9nXhV6eEyzEhd4qJQvAwhjunafPu8flR2KxGRGxeiwI7TVIBCA2a8MpXz
PBMR4CHEvUIoA0lV2GGyS2pbdwUZ3Va7Ry4S8Zp+67UzLBx83vynwIPC0fNQcnOe
IUOHfoMc7LpbjA4NUw11JI7mijOVLj8t3mLj5oqxaxFIsf/G9wmmj5EYJ1n28zmK
V7oXBzt2Q3OEHBgoOMg//h7PF4laTHJ6AU13eT3VmmQePNbYL3RswzbizRQ5Jqn1
WA5UzwTc0t1Iht+FSIi37NgFmo1tP8/xF++Q7KkVuYU/nYKo32x3xGGh4MrwDzhb
QJSpv5VUTUdKTMEzgqT1Bou+v83VMCbR1cxKF62WI1pH2FMxadrDI8VXzTAIE/0d
CIVW2eAiOIwknruOCvmk9Q7N7Mj8ELHW3ES9a5NHLnK2O97PI7g5ZzjzLGnVNKAM
7fZwWamONOVVtFhmz+8Fh8ERUatJFEVdddma3RBcki5+yVYyOvjOz+JachuGR3u5
nFvaKtSfTuHI7qg5T8KxAVvclVkLRSQ9cBYqLacCAwEAAaNTMFEwHQYDVR0OBBYE
FP7SoJvoVb//7YCNhKziXSslmlB5MB8GA1UdIwQYMBaAFP7SoJvoVb//7YCNhKzi
XSslmlB5MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggIBAAZ3H9XD
ue8FmB3gX+jJAMMEULFaqGG2sGikNuGByxfBV//VhjGdOv+dtPjWXs8kzzbAhdCV
ONUElzkrUuYRPl8z41zkQwLem7Gn9cDjR8i9cP6utAHI6IkQC03tO5INnWeb8bQ8
gJ9NRHjS7B4PWz1Z7akKNrW8Syrbr9DtbK3dkpY5tZpuiXVyQngAFzQAGwpuQJod
tSwpGZVYphhvT5B5IK+kYpUsxUbCBn0AdezEnnIjtK/SM1m85arOXic+rPYjXATo
S2uZImhfwdiDe15RNPsc++bzV+XwIgIDPtwsw4vTGCjMFRRsjX5XSimsV87rjuqR
kLaN5AKiMPtkpIr44Uc6hgvEW+zJXXKVDIJ30mXHRHAYzR5qCNLSmqDWAsNeT9WJ
ZH7MmWFto0oKGdTipWrvC0SwgmrWWlN6M/2LGdc/wVIichO53RbtXskWHi1IXytP
foo7/HDNVwozIcKPrsMOHCSbayS6QU7TAnrVfj1X3fpsisd2ODfEwoTH1UHtalyr
wxh0FRkFT1wYJkZWop4plOOO4VaHRAXlW1X3R1BUZ//d8BzkpcFHZkoxP0OUBBkP
IsTqMTXjKoX9bJsJuWP9kQ2TeSUhyF0/vVTxy5e/Yirvln4dSwT9AGoKdA/55Llb
7i5J6aa+BX7Vz6Y66aTXINFM73bIqHcCBWOF
-----END CERTIFICATE-----
)EOF";

// Сертификат клиента
const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEMjCCAhoCFC9YP80cbgHxzROy2FffzG3O2p8GMA0GCSqGSIb3DQEBCwUAMFMx
CzAJBgNVBAYTAlJVMQ8wDQYDVQQIDAZNb3Njb3cxDzANBgNVBAcMBk1vc2NvdzEP
MA0GA1UECgwGTGVudmVyMREwDwYDVQQDDAhMZW52ZXJDQTAeFw0yNjAzMDgwNjAz
MjNaFw0zNjAzMDUwNjAzMjNaMFgxCzAJBgNVBAYTAlJVMQ8wDQYDVQQIDAZNb3Nj
b3cxDzANBgNVBAcMBk1vc2NvdzEYMBYGA1UECgwPSG9tZSBNb25pdG9yaW5nMQ0w
CwYDVQQDDARwYXBhMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsDFm
JQzZXWb8D4kj6PgtGzcXngPpftZBVkQ0w+RcQWCeeZziLKoMa3JVFMOCN/LEGWln
ZdUvw7M1y755zS8qCsCjrAxVeJ3Ym0uZbOU6WJ+3wPfJVUAtyV3KLebMp6NH1e2r
Dmr6Eij+XFNDkVM3ixOGsVMF2Ve+Nk8KymsZbwCL8xdkQDajTegOBJYMvjPJ4nNO
8Gj/Oar4GuQrXh+3/vhnp5GT/VZdsGqerhUGAppQnJRFGWEsn1IeCg7S8x6iFURU
PEXEaHih70fOVEgXPaoqR4NVLhIC764Hc56H/IPrQLRWMW73DbGncL2EKrQC0LNz
HkBbAbZEkXmOY612HwIDAQABMA0GCSqGSIb3DQEBCwUAA4ICAQCOrFIMEyqtBGxV
OcGT5OZ741xFNY98eufsm/3cGD/BgklGMGu4ycuWGYWBWRQrgYXfwq3eKZV+/01V
F3RZCSQGBQwH/uDYGnhUb0KSOKq7eRz/mWWq77srKrE3pgiveX92olLqxCM4eZwK
4dUxYuqt1zGhXKfMRsJO05xrJJHjamaQsHRNnCivbMY9okhs025EqwTLQVB8XYWR
qtm1guyK8Mu4/0qX27NkWVtILC8vnTfqgsjlc4C0ooBq8sYcPwFOPpOUwEqp+z9P
TUmlhpZThE5rQFuX+UfY3rJj9EFx3SB8HiNZjzxRMQWC5aJm4IsWC9XDzvuqXghU
l+G90DzcnIcjYtbLmdF6Fcb/9N6F1RGmo5NQOH451LzoFNpxekdX3+TS5ONIxDQz
eAjt58tIi/Hc3eZOrjjtZ5AL2YOOx8A4TQPrY1IOs3u+ISTqqpW7293/B7iNGUuz
TNl0hfBRdWISGxTTsYRalhYpqPGoTEE1a7K9bmhfT0l9SxzMvHw7QWBb5oPeZ+tU
UOCxfPe9V/KHO6EdPkTwEIQCNTakVMsC6Mijz0UHYtZy5/FcK+B9pWoR3EC+KWpJ
1iz2/whsh5NIn0T22UwIILBXrRaiGH0C+5R6aT6DfaVm6SUv3vyBZAg9bGTveJ0d
0frTAWG3doKiuYbFO7bdM0Wtp/LSzA==
-----END CERTIFICATE-----
)EOF";

// Приватный ключ клиента
const char* client_key = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCwMWYlDNldZvwP
iSPo+C0bNxeeA+l+1kFWRDTD5FxBYJ55nOIsqgxrclUUw4I38sQZaWdl1S/DszXL
vnnNLyoKwKOsDFV4ndibS5ls5TpYn7fA98lVQC3JXcot5syno0fV7asOavoSKP5c
U0ORUzeLE4axUwXZV742TwrKaxlvAIvzF2RANqNN6A4Elgy+M8nic07waP85qvga
5CteH7f++GenkZP9Vl2wap6uFQYCmlCclEUZYSyfUh4KDtLzHqIVRFQ8RcRoeKHv
R85USBc9qipHg1UuEgLvrgdznof8g+tAtFYxbvcNsadwvYQqtALQs3MeQFsBtkSR
eY5jrXYfAgMBAAECggEAATUslmZtXo35msdvUM0o6f3XrtKZJz6jnR+nOrcvAV7U
ECYBs8RzhK0Gurdfd+i1UBGAVgyw0pHPIEHX36stpkM3/4taGUwck221yWfbD050
hIv8fBXjiK21H6CI0Xo8tbUFA3qer4gR9WLwuRH02IJsiEO+MxOFctBLqk5sEsN6
Da+k5uczl3ZldgVZyB6ByAZdt4zNSyyrJEdTGa7T8Zb834JfebAhThnV+A+J8XrQ
SPSE41iFEhsV9RQ3dFr/i0+BuOaKYYeImVJMn83Q57qzPQc1Itmb/d8iS9VFYi17
pAmbTsJDnuzDtdi4DydWrGas2Mqj+LBHjAtV4UMI6QKBgQDlNnvwcGG/B4xvRQFM
qW43Ksass1D1otmpo+fFOgOAYEyz4LeWOi7PO3enKHmpSfoankJC8HwfH/9OWqqj
edpA8w1fPRKg/jRSyA7LWVTtINMybbHexN1ru6ucXdtW7/TmpFLuuJgg8EfZuYIO
6U+Ji2lvhtPG506xQ1ZZZ72+7QKBgQDEyK7/4P7X/g3YIZR7nlOnsI/YC2i7ZzkY
vJ/tQpuTQhLWwypaLH1LdcFZF7Y1Ep1OP/etg/gASXnoGIdnT9n/MVGot+EK7xib
6l4VmcHQhYqfnKag3Yqa6w6rf+EEqOgmAAfdKg+LSlckbnyq1TTUWHdWBUI58Fge
K4Fn7rAbuwKBgADn5Td/CjPaEMkDycyGwuO+7IgUMVa3LQsJlcnmv8rLFqKDWdZj
Jno1LvTfKRPtMerXA0YGc0cgNj8WBqScv6sQCtZ8hE1Uk9pb5gsRsE87mPoTCufp
eUKKvFPi3xIJ4rrinV1KJ3fB21bV79ilPWY6wdDqBH49vcbfkEDMMyjFAoGAEtIO
yrI+LXJC6LTtepU98v/UAhm1Ppc0wsTnls2UC5V6L8giuNUgdqMghuDZsGHDYcSE
b7f7dCuS2TvfV2Yvt1SENtq/T0hm1iKoNPCrtMHQ7EsTNLd7li19DbIBV2zAJ0JB
fkmTiwLlKrrWeZBCqTx8zWFbYCc4hkoq6kHN3hkCgYBvF8lkxg3/c/3w+ZKNhJOc
2GDHSqXDpcoUXgn/hBdN650oGijFoUD35ctVHYQerQbZW7iHszwpHXTtR7m511gm
dyPqLyZxDGyWqEe3peQrWEbyd2aH41MO6C+iO1z7rXGk5XX329bT7nmsIvoEZciN
xfZHRFNRALvnDe9usRMQBw==
-----END PRIVATE KEY-----
)EOF";

// Безопасный WiFi клиент для TLS
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

extern int hiveId;
extern float currentWeight;
extern float currentTemperature;
extern float currentBattery;
extern bool isLogEnabled;

// ========== ИНИЦИАЛИЗАЦИЯ MQTT ==========
void initMQTT() {
    // Настройка TLS сертификатов
    secureClient.setCACert(ca_cert);
    secureClient.setCertificate(client_cert);
    secureClient.setPrivateKey(client_key);
    
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setBufferSize(512);
    LOG_W("MQTT", "MQTT-TLS initialized with server %s:%d", MQTT_SERVER, MQTT_PORT);
}

// ========== ПОДКЛЮЧЕНИЕ К MQTT ==========
void connectMQTT() {
    static uint32_t lastTry = 0;
    const uint32_t MQTT_CONNECT_TIMEOUT = 10000;

    if (mqttClient.connected()) return;
    if (millis() - lastTry < MQTT_RECONNECT_DELAY) return;

    lastTry = millis();

    String clientId = "Hive-";
    clientId += String(hiveId);
    clientId += "-";
    clientId += String(random(0xffff), HEX);

    LOG_W("MQTT", "Connecting to MQTT-TLS as %s...", clientId.c_str());

    bool connected = false;
    unsigned long connectStart = millis();

    while (!connected && (millis() - connectStart < MQTT_CONNECT_TIMEOUT)) {
        // Подключение без логина/пароля - аутентификация по сертификату
        connected = mqttClient.connect(clientId.c_str());
        if (!connected) delay(100);
    }

    if (connected) {
        LOG_W("MQTT", "✅ Connected to MQTT broker via TLS");
    } else {
        LOG_W("MQTT", "❌ TLS Connection failed (state: %d)", mqttClient.state());
    }
}

// ========== ПУБЛИКАЦИЯ ДАННЫХ ==========
void publishHiveData() {
    if (!mqttClient.connected()) {
        LOG_W("MQTT", "Cannot publish - not connected");
        return;
    }

    JsonDocument doc;
    doc["hive_id"] = hiveId;
    doc["weight"] = currentWeight;
    doc["temp_in"] = currentTemperature;
    doc["bat"] = currentBattery;
    
    float battPercent = 0;
    if (currentBattery >= 4.0) battPercent = 100;
    else if (currentBattery <= 3.3) battPercent = 0;
    else battPercent = ((currentBattery - 3.3) / 0.7) * 100;
    
    doc["bat_pct"] = battPercent;
    doc["rssi"] = WiFi.RSSI();
    
    String payload;
    serializeJson(doc, payload);

    LOG_W("MQTT", "📤 Publishing to %s", MQTT_TOPIC_HIVE);
    
    if (mqttClient.publish(MQTT_TOPIC_HIVE, payload.c_str(), false)) {
        LOG_W("MQTT", "✅ Published successfully");
    } else {
        LOG_W("MQTT", "❌ Publish failed");
    }
    delay(100);
}

// ========== ОБРАБОТКА MQTT ==========
void handleMQTT() {
    if (!mqttClient.connected()) connectMQTT();
    if (mqttClient.connected()) mqttClient.loop();
}

#endif