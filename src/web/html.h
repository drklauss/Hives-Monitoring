#ifndef WEB_HTML_H
#define WEB_HTML_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>🐝 Hive Monitor</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <main class="container">
        <h1>🐝 Улей #{HIVE_ID}</h1>
        
        <!-- Секция датчиков -->
        <article>
            <h2>📊 Текущие показания</h2>
            <div class="grid-2">
                <div class="sensor-card">
                    <div class="sensor-value" id="weight">{WEIGHT}</div>
                    <div class="sensor-label">Вес (кг)</div>
                </div>
                <div class="sensor-card">
                    <div class="sensor-value" id="temp">{TEMP}</div>
                    <div class="sensor-label">Температура (°C)</div>
                </div>
                <div class="sensor-card">
                    <div class="sensor-value" id="hum">{HUM}</div>
                    <div class="sensor-label">Влажность (%)</div>
                </div>
                <div class="sensor-card">
                    <div class="sensor-value" id="bat">{BATT}</div>
                    <div class="sensor-label">Батарея (В)</div>
                </div>
            </div>
        </article>

        <!-- Настройки улья -->
        <article>
            <h2>⚙️ Настройки</h2>
            <form id="hiveForm">
                <div class="grid-2">
                    <div>
                        <label for="hive_id">ID улья</label>
                        <input type="number" id="hive_id" name="hive_id" min="1" max="99" value="{HIVE_ID}" required>
                    </div>
                    <div>
                        <label for="sleep_interval">Интервал сна (сек)</label>
                        <input type="number" id="sleep_interval" name="sleep_interval" min="60" max="86400" value="{SLEEP}" required>
                    </div>
                </div>
                <button type="submit">💾 Сохранить</button>
            </form>
        </article>

        <!-- Калибровка весов -->
        <article>
            <h2>⚖️ Калибровка весов</h2>
            
            <div class="grid-2">
                <div>
                    <p><strong>Шаг 1:</strong> Убери любой груз с площадки</p>
                    <button id="tareBtn" class="secondary">📌 Калибровать SCALE_OFFSET (тара)</button>
                </div>
                <div>
                    <p><strong>Сырое значение АЦП:</strong></p>
                    <div class="raw-value" id="raw">---</div>
                    <span class="text-muted">Текущие показания датчика до преобразования в кг</span>
                </div>
            </div>
            
            <hr>
            
            <p><strong>Шаг 2:</strong> Положи груз известного веса</p>
            <div class="grid-2">
                <input type="number" id="knownWeight" step="0.1" placeholder="Вес груза (кг)">
                <button id="calibrateBtn">⚖️ Калибровать SCALE_FACTOR</button>
            </div>
            
            <details>
                <summary>Текущие параметры калибровки</summary>
                <div class="grid-2">
                    <div>Offset (тара): <strong id="currentOffset">---</strong></div>
                    <div>Factor: <strong id="currentFactor">---</strong></div>
                </div>
            </details>
        </article>

        <!-- WiFi настройки -->
        <article>
            <h2>📶 WiFi</h2>
            <button id="scanBtn" class="secondary">🔍 Сканировать сети</button>
            <div id="networks" class="network-list"></div>
            
            <form id="wifiForm">
                <div class="grid-2">
                    <div>
                        <label for="ssid">SSID</label>
                        <input type="text" id="ssid" required placeholder="Введите название сети">
                    </div>
                    <div>
                        <label for="password">Пароль</label>
                        <input type="password" id="password" placeholder="Введите пароль">
                    </div>
                </div>
                <button type="submit">📡 Сохранить и перезагрузить</button>
            </form>
        </article>

        <div id="status"></div>
    </main>
    <script src="/script.js"></script>
</body>
</html>
)rawliteral";

#endif