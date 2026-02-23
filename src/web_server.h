#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include <ESPmDNS.h>
#include "config.h"
#include "settings.h"
#include "wifi_manager.h" 

WebServer server(80);
DNSServer dnsServer;

// Прототипы функций
void handleRoot();
void handleSetupPage();
void handleCaptivePortal();
void handleSave();
void handleSetLogLevel();
void handleRestart();
void handleWiFiScan();
void handleWiFiConnect();
void handleOTAMode();
void handleOTAStatus();
void initWebServer();
void handleWebServer();

// Внешние переменные
extern int hiveId;
extern float currentWeight;
extern float currentTemperature;
extern float currentBattery;
extern uint32_t sleepInterval;
extern bool isLogEnabled;        // ← исправлено с uint8_t на bool
extern bool forceOTAMode;

// MARK: mainHTML
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP Hive Monitor</title>
    
    <!-- Bootstrap 5 CSS и иконки -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.0/font/bootstrap-icons.css">
    
    <style>
        body { transition: background-color 0.3s ease, color 0.3s ease; }
        .theme-toggle {
            cursor: pointer;
            padding: 0.5rem;
            border-radius: 50%;
            transition: background-color 0.3s ease;
        }
        .theme-toggle:hover { background-color: rgba(128, 128, 128, 0.1); }
        .card { transition: background-color 0.3s ease, border-color 0.3s ease; }
        .spinner {
            display: inline-block;
            width: 1rem;
            height: 1rem;
            border: 2px solid currentColor;
            border-right-color: transparent;
            border-radius: 50%;
            animation: spinner 0.75s linear infinite;
        }
        @keyframes spinner { to { transform: rotate(360deg); } }
        .battery-good { color: #28a745; }
        .battery-warning { color: #ffc107; }
        .battery-critical { color: #dc3545; }
    </style>
</head>
<body data-bs-theme="light">

    <!-- Навигационная панель -->
    <nav class="navbar navbar-expand-lg bg-body-tertiary mb-4">
        <div class="container">
            <a class="navbar-brand" href="#">
                <i class="bi bi-house-heart-fill me-2"></i>
                ESP Hive Monitor
            </a>
            
            <div class="d-flex align-items-center">
                <div class="theme-toggle me-3" onclick="toggleTheme()" title="Сменить тему">
                    <i class="bi bi-sun-fill" id="themeIcon"></i>
                </div>
                <span class="navbar-text">
                    <small class="text-body-secondary">v)" FIRMWARE_VERSION R"rawliteral(</small>
                </span>
            </div>
        </div>
    </nav>

    <div class="container">
        <!-- Карточки со статистикой -->
        <div class="row g-4 mb-5">
            <!-- Вес -->
            <div class="col-md-3">
                <div class="card shadow-sm h-100">
                    <div class="card-body text-center p-4">
                        <div class="display-4 text-primary mb-3">
                            <i class="bi bi-thermometer-half"></i>
                        </div>
                        <h6 class="text-body-secondary mb-2">ВЕС УЛЬЯ</h6>
                        <h2 class="display-5 fw-bold mb-0" id="weight">---</h2>
                        <p class="text-body-secondary">кг</p>
                    </div>
                </div>
            </div>

            <!-- Температура -->
            <div class="col-md-3">
                <div class="card shadow-sm h-100">
                    <div class="card-body text-center p-4">
                        <div class="display-4 text-primary mb-3">
                            <i class="bi bi-thermometer-half"></i>
                        </div>
                        <h6 class="text-body-secondary mb-2">ТЕМПЕРАТУРА</h6>
                        <h2 class="display-5 fw-bold mb-0" id="temperature">---</h2>
                        <p class="text-body-secondary">°C</p>
                    </div>
                </div>
            </div>

            <!-- Батарея -->
            <div class="col-md-3">
                <div class="card shadow-sm h-100">
                    <div class="card-body text-center p-4">
                        <div class="display-4 text-primary mb-3">
                            <i class="bi bi-battery-charging"></i>
                        </div>
                        <h6 class="text-body-secondary mb-2">БАТАРЕЯ</h6>
                        <h2 class="display-5 fw-bold mb-0" id="battery">---</h2>
                        <p class="text-body-secondary">В</p>
                    </div>
                </div>
            </div>

            <!-- Качество сигнала -->
            <div class="col-md-3">
                <div class="card shadow-sm h-100">
                    <div class="card-body text-center p-4">
                        <div class="display-4 text-primary mb-3">
                            <i class="bi bi-wifi"></i>
                        </div>
                        <h6 class="text-body-secondary mb-2">СИГНАЛ</h6>
                        <h2 class="display-5 fw-bold mb-0" id="rssi">---</h2>
                        <p class="text-body-secondary">dBm</p>
                    </div>
                </div>
            </div>
        </div>

        <!-- Основная карточка с табами -->
        <div class="card shadow-sm border-0 mb-5">
            <div class="card-body p-4">
                <!-- Навигация по табам -->
                <ul class="nav nav-tabs nav-fill mb-4">
                    <li class="nav-item">
                        <button class="nav-link active" data-bs-toggle="tab" data-bs-target="#hive">
                            <i class="bi bi-house-heart me-2"></i>Улей
                        </button>
                    </li>
                    <li class="nav-item">
                        <button class="nav-link" data-bs-toggle="tab" data-bs-target="#wifi">
                            <i class="bi bi-wifi me-2"></i>WiFi
                        </button>
                    </li>
                    <li class="nav-item">
                        <button class="nav-link" data-bs-toggle="tab" data-bs-target="#system">
                            <i class="bi bi-hdd-stack me-2"></i>Система
                        </button>
                    </li>
                </ul>

                <div class="tab-content">
                   <!-- ТАБ 1: Настройки улья -->
                    <div class="tab-pane fade show active" id="hive">
                        <form id="hiveForm" onsubmit="saveSettings(event)">
                            <div class="row g-4">
                                <div class="col-md-6">
                                    <label class="form-label text-body-secondary">
                                        <i class="bi bi-hash me-2"></i>Номер улья
                                    </label>
                                    <input type="number" class="form-control" name="hive_id" 
                                        min="1" max="99" value="{HIVE_ID}" required>
                                </div>
                                
                                <div class="col-md-6">
                                    <label class="form-label text-body-secondary">
                                        <i class="bi bi-clock-history me-2"></i>Интервал пробуждения
                                    </label>
                                    <div class="input-group">
                                        <input type="number" class="form-control" name="sleep_interval" 
                                            min="300" max="86400" value="{SLEEP_INTERVAL}" required>
                                        <span class="input-group-text">сек</span>
                                    </div>
                                    <small class="text-body-secondary">
                                        Мин: 5 мин, Макс: 24 часа
                                    </small>
                                </div>
                                
                                <div class="col-md-6">
                                    <label class="form-label text-body-secondary">
                                        <i class="bi bi-calibration me-2"></i>Калибровка весов
                                    </label>
                                    <div class="input-group">
                                        <input type="number" class="form-control" name="scale_factor" 
                                            step="0.1" value="{SCALE_FACTOR}" required>
                                        <span class="input-group-text">г/ед</span>
                                    </div>
                                    <small class="text-body-secondary">
                                        Отрицательное значение для тензодатчика
                                    </small>
                                </div>
                            </div>
                            
                            <div class="alert alert-info mt-4" role="alert">
                                <i class="bi bi-moon-stars me-2"></i>
                                Устройство работает в режиме глубокого сна. 
                                После сохранения настроек оно перезагрузится и уснет на указанный интервал.
                            </div>
                            
                            <div class="text-end mt-4">
                                <button type="submit" class="btn btn-primary px-5">
                                    <i class="bi bi-check-lg me-2"></i>Сохранить и перезагрузить
                                </button>
                            </div>
                        </form>
                    </div>

                    <!-- ТАБ 2: WiFi (без изменений) -->
                    <div class="tab-pane fade" id="wifi">
                        <!-- ... тот же код, что и в проекте со светом ... -->
                    </div>

                    <!-- ТАБ 3: Система (с добавлением информации о батарее) -->
                    <div class="tab-pane fade" id="system">
                        <div class="row g-4">
                            <div class="col-md-6">
                                <h5 class="mb-3"><i class="bi bi-info-circle me-2"></i>Информация</h5>
                                <div class="list-group">
                                    <div class="list-group-item d-flex justify-content-between align-items-center">
                                        <span><i class="bi bi-clock me-2"></i>Время работы</span>
                                        <span class="badge bg-primary rounded-pill" id="uptime">---</span>
                                    </div>
                                    <div class="list-group-item d-flex justify-content-between align-items-center">
                                        <span><i class="bi bi-hdd-network me-2"></i>IP адрес</span>
                                        <span class="badge bg-secondary rounded-pill" id="ip">---</span>
                                    </div>
                                    <div class="list-group-item d-flex justify-content-between align-items-center">
                                        <span><i class="bi bi-memory me-2"></i>Свободно памяти</span>
                                        <span class="badge bg-info rounded-pill" id="freeHeap">---</span>
                                    </div>
                                    <div class="list-group-item d-flex justify-content-between align-items-center">
                                        <span><i class="bi bi-battery-charging me-2"></i>Заряд батареи</span>
                                        <span class="badge rounded-pill" id="batteryPercent">---</span>
                                    </div>
                                    <div class="list-group-item d-flex justify-content-between align-items-center">
                                        <span><i class="bi bi-tag me-2"></i>Версия</span>
                                        <span class="badge bg-secondary rounded-pill">)" FIRMWARE_VERSION R"rawliteral(</span>
                                    </div>
                                    <div class="list-group-item d-flex justify-content-between align-items-center">
                                        <span><i class="bi bi-terminal me-2"></i>Уровень логов</span>
                                        <span class="badge bg-secondary rounded-pill" id="isLogEnabledDisplay">---</span>
                                    </div>
                                </div>
                            </div>
                            
                            <div class="col-md-6">
                                <h5 class="mb-3"><i class="bi bi-gear me-2"></i>Управление</h5>
                                <div class="list-group">
                                    <div class="list-group-item">
                                        <label class="form-label fw-bold">Уровень логирования</label>
                                        <select class="form-select" id="logLevelSelector" onchange="changeLogLevel(this.value)">
                                            <option value="0">🔇 Выключено</option>
                                            <option value="1">🔴 Ошибки</option>
                                            <option value="2" selected>🟢 Информация</option>
                                            <option value="3">🔵 Отладка</option>
                                        </select>
                                    </div>
                                    <!-- В таб System, после управления уровнем логирования -->
                                    <div class="list-group-item">
                                        <div class="d-flex justify-content-between align-items-center mb-2">
                                            <span><i class="bi bi-cloud-upload me-2"></i>OTA режим</span>
                                            <span class="badge bg-warning rounded-pill" id="otaStatus">Выкл</span>
                                        </div>
                                        <button class="btn btn-warning w-100" onclick="enableOTAMode()">
                                            <i class="bi bi-cloud-upload me-2"></i>Включить OTA режим (не спать)
                                        </button>
                                        <small class="text-body-secondary d-block mt-2">
                                            Устройство не будет уходить в сон до следующей перезагрузки
                                        </small>
                                    </div>
                                    <div class="list-group-item">
                                        <button class="btn btn-outline-danger w-100" onclick="restartESP()">
                                            <i class="bi bi-arrow-repeat me-2"></i>Перезагрузить
                                        </button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <footer class="text-center text-body-secondary mb-4">
            <small>ESP32-C3 Hive Monitor • Данные отправляются раз в час</small>
        </footer>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
    <script>
        // Функции темы
        function toggleTheme() {
            const html = document.body;
            const currentTheme = html.getAttribute('data-bs-theme');
            const newTheme = currentTheme === 'light' ? 'dark' : 'light';
            html.setAttribute('data-bs-theme', newTheme);
            document.getElementById('themeIcon').className = newTheme === 'light' ? 'bi bi-sun-fill' : 'bi bi-moon-stars-fill';
            localStorage.setItem('theme', newTheme);
        }
        
        function loadTheme() {
            const savedTheme = localStorage.getItem('theme') || 'light';
            document.body.setAttribute('data-bs-theme', savedTheme);
            document.getElementById('themeIcon').className = savedTheme === 'light' ? 'bi bi-sun-fill' : 'bi bi-moon-stars-fill';
        }
        
        // Обновление статистики
        function updateStats() {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('weight').textContent = data.weight.toFixed(1);
                    document.getElementById('temperature').textContent = data.temp.toFixed(1);
                    document.getElementById('battery').textContent = data.battery.toFixed(2);
                    document.getElementById('rssi').textContent = data.rssi;
                    
                    // Процент батареи с цветом
                    const battPercent = document.getElementById('batteryPercent');
                    battPercent.textContent = data.battPct.toFixed(0) + '%';
                    battPercent.className = 'badge rounded-pill ';
                    if (data.battPct > 50) battPercent.classList.add('bg-success');
                    else if (data.battPct > 20) battPercent.classList.add('bg-warning');
                    else battPercent.classList.add('bg-danger');
                    
                    // Uptime
                    let uptime = '';
                    const days = Math.floor(data.uptime / 86400);
                    const hours = Math.floor((data.uptime % 86400) / 3600);
                    const mins = Math.floor((data.uptime % 3600) / 60);
                    if (days > 0) uptime += days + 'д ';
                    uptime += hours + 'ч ' + mins + 'м';
                    document.getElementById('uptime').textContent = uptime;
                    
                    document.getElementById('ip').textContent = data.ip;
                    document.getElementById('freeHeap').textContent = data.freeHeap + ' KB';
                    
                    // Уровень логов
                    const levels = ['🔇 Выключено', '🔴 Ошибки', '🟢 Информация', '🔵 Отладка'];
                    document.getElementById('isLogEnabledDisplay').textContent = levels[data.logLevel];
                    document.getElementById('logLevelSelector').value = data.logLevel;
                });
        }
        
        function changeLogLevel(level) {
            fetch('/setLogLevel', {method:'POST', body:new URLSearchParams({level:level})})
                .then(() => alert('✅ Уровень логирования изменен. Перезагрузите устройство.'));
        }
        
        function restartESP() {
            if (confirm('Перезагрузить устройство?')) {
                fetch('/restart').then(() => setTimeout(() => location.reload(), 5000));
            }
        }

        // Проверка статуса OTA режима
        fetch('/otastatus')
            .then(r => r.json())
            .then(data => {
                const otaStatus = document.getElementById('otaStatus');
                if (data.otaMode) {
                    otaStatus.textContent = 'Вкл';
                    otaStatus.className = 'badge bg-success rounded-pill';
                } else {
                    otaStatus.textContent = 'Выкл';
                    otaStatus.className = 'badge bg-warning rounded-pill';
                }
            });

        // Включение OTA режима
        function enableOTAMode() {
            if (confirm('Включить OTA режим? Устройство не будет спать до перезагрузки')) {
                fetch('/otamode', {method: 'POST'})
                    .then(r => r.text())
                    .then(msg => {
                        alert(msg);
                        setTimeout(() => location.reload(), 2000);
                    });
            }
        }
        
       document.getElementById('hiveForm').onsubmit = function(e) {
            e.preventDefault();
            const formData = new FormData(this);
            const data = new URLSearchParams(formData);
            
            fetch('/save', {
                method: 'POST',
                body: data
            })
            .then(response => response.text())
            .then(message => {
                alert(message);
                if (message.includes('restart')) {
                    setTimeout(() => window.location.reload(), 3000);
                }
            });
        };
        
        loadTheme();
        updateStats();
        setInterval(updateStats, 5000);
    </script>
</body>
</html>
)rawliteral";

// MARK: handleSetupPage
void handleSetupPage() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <title>ESP Hive - Настройка WiFi</title>
        <style>
            * { margin: 0; padding: 0; box-sizing: border-box; }
            body { 
                font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Arial, sans-serif;
                background: #f5f5f5;
                min-height: 100vh;
                display: flex;
                align-items: center;
                justify-content: center;
                padding: 20px;
            }
            .container { max-width: 500px; width: 100%; }
            .card {
                background: white;
                border-radius: 10px;
                padding: 30px;
                box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            }
            h2 {
                color: #333;
                margin-bottom: 25px;
                font-size: 1.5rem;
                text-align: center;
            }
            .form-group { margin-bottom: 20px; }
            label {
                display: block;
                margin-bottom: 8px;
                color: #555;
                font-weight: 500;
                font-size: 0.9rem;
            }
            input, select {
                width: 100%;
                padding: 10px;
                border: 1px solid #ddd;
                border-radius: 5px;
                font-size: 1rem;
            }
            input:focus { outline: none; border-color: #2196F3; }
            .btn {
                background: #2196F3;
                color: white;
                border: none;
                padding: 12px;
                border-radius: 5px;
                font-size: 1rem;
                cursor: pointer;
                width: 100%;
            }
            .btn:hover { background: #1976D2; }
            .btn:disabled { background: #ccc; cursor: not-allowed; }
            .btn-secondary {
                background: #f0f0f0;
                color: #333;
                border: 1px solid #ddd;
            }
            .btn-secondary:hover { background: #e0e0e0; }
            .network-list {
                border: 1px solid #ddd;
                border-radius: 5px;
                margin: 15px 0;
                max-height: 250px;
                overflow-y: auto;
            }
            .network-item {
                padding: 10px 15px;
                border-bottom: 1px solid #eee;
                cursor: pointer;
                display: flex;
                justify-content: space-between;
                align-items: center;
            }
            .network-item:last-child { border-bottom: none; }
            .network-item:hover { background: #f8f9fa; }
            .network-item.selected { background: #e3f2fd; }
            .network-name { font-weight: 500; }
            .network-info { color: #666; font-size: 0.85rem; }
            .status {
                margin-top: 15px;
                padding: 10px;
                border-radius: 5px;
                background: #f8f9fa;
                color: #666;
                font-size: 0.9rem;
                display: none;
            }
            .status.success { background: #d4edda; color: #155724; display: block; }
            .status.error { background: #f8d7da; color: #721c24; display: block; }
            .status.info { background: #d1ecf1; color: #0c5460; display: block; }
            .text-center { text-align: center; }
            .mb-3 { margin-bottom: 15px; }
            .mt-3 { margin-top: 15px; }
            .small { font-size: 0.85rem; color: #666; }
            .spinner {
                display: inline-block;
                width: 16px;
                height: 16px;
                border: 2px solid #ccc;
                border-top-color: #333;
                border-radius: 50%;
                animation: spin 0.8s linear infinite;
            }
            @keyframes spin { to { transform: rotate(360deg); } }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="card">
                <h2>🔧 Настройка WiFi</h2>
                
                <div class="text-center mb-3">
                    <button class="btn btn-secondary" onclick="scanNetworks()" id="scanBtn">
                        🔍 Сканировать сети
                    </button>
                </div>
                
                <div id="networksContainer" style="display: none;">
                    <div class="network-list" id="networksList"></div>
                </div>
                
                <form id="wifiForm" onsubmit="connectWiFi(event)">
                    <div class="form-group">
                        <label>Название сети (SSID)</label>
                        <input type="text" id="ssid" name="ssid" required 
                               placeholder="Введите или выберите сеть">
                    </div>
                    
                    <div class="form-group">
                        <label>Пароль</label>
                        <input type="password" id="password" name="password" 
                               placeholder="Введите пароль (если есть)">
                    </div>
                    
                    <button type="submit" class="btn">📡 Подключиться</button>
                </form>
                
                <div class="status" id="statusMessage"></div>
                <div class="small text-center mt-3">
                    Точка доступа: <strong>ESP-Hive-Setup</strong>
                </div>
            </div>
        </div>

        <script>
            function scanNetworks() {
                const btn = document.getElementById('scanBtn');
                btn.innerHTML = '<span class="spinner"></span> Сканирование...';
                btn.disabled = true;
                
                fetch('/scan')
                    .then(r => r.json())
                    .then(networks => {
                        let html = '';
                        networks.forEach(net => {
                            let lock = net.encrypted ? '🔒' : '🔓';
                            let signal = net.rssi > -60 ? '📶📶📶' : net.rssi > -75 ? '📶📶' : '📶';
                            html += `
                                <div class="network-item" onclick="selectNetwork('${net.ssid}')">
                                    <span class="network-name">${net.ssid}</span>
                                    <span class="network-info">${signal} ${net.rssi}dBm ${lock}</span>
                                </div>
                            `;
                        });
                        
                        document.getElementById('networksList').innerHTML = html;
                        document.getElementById('networksContainer').style.display = 'block';
                        showStatus('✅ Найдено сетей: ' + networks.length, 'success');
                    })
                    .catch(() => showStatus('❌ Ошибка сканирования', 'error'))
                    .finally(() => {
                        btn.innerHTML = '🔍 Сканировать сети';
                        btn.disabled = false;
                    });
            }
            
            function selectNetwork(ssid) {
                document.getElementById('ssid').value = ssid;
                document.querySelectorAll('.network-item').forEach(i => i.classList.remove('selected'));
                event.currentTarget.classList.add('selected');
            }
            
            function connectWiFi(e) {
                e.preventDefault();
                
                const ssid = document.getElementById('ssid').value.trim();
                if (!ssid) {
                    showStatus('❌ Введите название сети', 'error');
                    return;
                }
                
                showStatus('⏳ Подключение...', 'info');
                
                const data = new URLSearchParams();
                data.append('ssid', ssid);
                data.append('password', document.getElementById('password').value);
                
                fetch('/connect', { method: 'POST', body: data })
                    .then(r => r.text())
                    .then(html => { document.open(); document.write(html); document.close(); })
                    .catch(() => showStatus('❌ Ошибка подключения', 'error'));
            }
            
            function showStatus(msg, type) {
                const el = document.getElementById('statusMessage');
                el.textContent = msg;
                el.className = 'status ' + type;
            }
            
            window.onload = () => setTimeout(scanNetworks, 500);
        </script>
    </body>
    </html>
    )rawliteral";
    
    server.send(200, "text/html", html);
}


// MARK: handlers
void handleRoot() {
    String html = FPSTR(INDEX_HTML);
    
    // Подставляем текущие значения в инпуты
    html.replace("{HIVE_ID}", String(hiveId));
    html.replace("{SLEEP_INTERVAL}", String(sleepInterval));
    html.replace("{SCALE_FACTOR}", String(scaleFactor, 1));
    
    server.send(200, "text/html", html);
}

// Перенаправляем на страницу настройки
void handleCaptivePortal() {
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/setup", true);
    server.send(302, "text/plain", "");
}

void handleSave() {
    bool changed = false;
    
    if (server.hasArg("hive_id") && server.arg("hive_id") != "") {
        int newId = server.arg("hive_id").toInt();
        if (newId >= 1 && newId <= 99 && newId != hiveId) {
            hiveId = newId;
            changed = true;
        }
    }
    
    if (server.hasArg("sleep_interval") && server.arg("sleep_interval") != "") {
        uint32_t newInterval = server.arg("sleep_interval").toInt();
        if (newInterval >= MIN_SLEEP_INTERVAL && newInterval <= MAX_SLEEP_INTERVAL && 
            newInterval != sleepInterval) {
            sleepInterval = newInterval;
            changed = true;
        }
    }
    
    if (server.hasArg("scale_factor") && server.arg("scale_factor") != "") {
        float newScale = server.arg("scale_factor").toFloat();
        if (newScale != scaleFactor) {
            scaleFactor = newScale;
            changed = true;
        }
    }
    
    if (changed) {
        saveSettings();
        server.send(200, "text/plain", "✅ Settings saved. Device will restart...");
        delay(100);
        ESP.restart();
    } else {
        server.send(200, "text/plain", "No changes detected");
    }
}

void handleSetLogLevel() {
    if (server.hasArg("level")) {
        bool newLevel = server.arg("level").toInt() > 0;  // конвертируем в bool
        if (newLevel != isLogEnabled) {
            isLogEnabled = newLevel;
            saveSettings();
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Invalid level");
        }
    }
}

void handleRestart() {
    server.send(200, "text/plain", "Restarting...");
    delay(100);
    ESP.restart();
}

void handleWiFiScan() {
    scanWiFiNetworks();  // вызываем функцию из wifi_manager.h
}

void handleWiFiConnect() {
    if (!server.hasArg("ssid") || !server.hasArg("password")) {
        server.send(400, "text/plain", "Missing ssid or password");
        return;
    }
    
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    if (ssid.length() == 0) {
        server.send(400, "text/plain", "SSID cannot be empty");
        return;
    }
    
    connectToWiFiNetwork(ssid, password);  // из wifi_manager.h
}

void handleOTAMode() {
    forceOTAMode = true;
    saveSettings();
    server.send(200, "text/plain", "✅ OTA mode activated");
    LOG_W("WEB", "OTA mode activated via web interface");
}

void handleOTAStatus() {
    String json = "{\"otaMode\":" + String(forceOTAMode ? "true" : "false") + "}";
    server.send(200, "application/json", json);
}

void initWebServer() {
    server.on("/", handleRoot);
    server.on("/setup", handleSetupPage);
    server.on("/save", HTTP_POST, handleSave);
    server.on("/setLogLevel", HTTP_POST, handleSetLogLevel);
    server.on("/restart", handleRestart);
    server.on("/scan", handleWiFiScan);
    server.on("/connect", HTTP_POST, handleWiFiConnect);
    server.on("/otamode", HTTP_POST, handleOTAMode);
    server.on("/otastatus", handleOTAStatus);
    server.onNotFound(handleCaptivePortal);
    
    server.begin();
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    
    LOG_W("WEB", "✅ Web server started");
}

void handleWebServer() {
    server.handleClient();
    dnsServer.processNextRequest();
}

#endif