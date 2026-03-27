#ifndef WEB_JS_H
#define WEB_JS_H

#include <Arduino.h>

const char SCRIPT_JS[] PROGMEM = R"rawliteral(
// Обновление датчиков (вес, температура, влажность, батарея)
async function updateSensors() {
    try {
        const res = await fetch('/sensors');
        const data = await res.json();
        
        document.getElementById('weight').textContent = data.weight.toFixed(2);
        document.getElementById('temp').textContent = data.temp.toFixed(1);
        document.getElementById('hum').textContent = data.humidity.toFixed(0);
        document.getElementById('bat').textContent = data.battery.toFixed(2);
    } catch (e) {
        console.error('Sensor update failed:', e);
    }
}

// Обновление калибровки (raw, offset, factor)
async function updateCalibration() {
    try {
        const res = await fetch('/getCalibration');
        const data = await res.json();
        
        document.getElementById('raw').textContent = data.raw;
        document.getElementById('currentOffset').textContent = data.offset;
        document.getElementById('currentFactor').textContent = data.factor.toFixed(1);
    } catch (e) {
        console.error('Calibration update failed:', e);
    }
}

// Сохранение настроек улья
document.getElementById('hiveForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const data = new URLSearchParams({
        hive_id: document.getElementById('hive_id').value,
        sleep_interval: document.getElementById('sleep_interval').value
    });
    
    const res = await fetch('/saveHive', { method: 'POST', body: data });
    const msg = await res.text();
    showStatus(msg, 'success');
    setTimeout(() => location.reload(), 2000);
});

// Калибровка offset (тара)
document.getElementById('tareBtn').addEventListener('click', async () => {
    const res = await fetch('/setOffset', { method: 'POST' });
    const msg = await res.text();
    showStatus(msg, 'success');
    setTimeout(() => location.reload(), 1500);
});

// Калибровка factor (коэффициент)
document.getElementById('calibrateBtn').addEventListener('click', async () => {
    const weight = document.getElementById('knownWeight').value;
    if (!weight || weight <= 0) {
        showStatus('❌ Введите вес груза', 'error');
        return;
    }
    
    const res = await fetch('/setFactor', { 
        method: 'POST', 
        body: new URLSearchParams({weight: weight})
    });
    const msg = await res.text();
    showStatus(msg, 'success');
    setTimeout(() => location.reload(), 1500);
});

// Сканирование WiFi
document.getElementById('scanBtn').addEventListener('click', async () => {
    const btn = document.getElementById('scanBtn');
    btn.textContent = '⏳ Сканирование...';
    btn.disabled = true;
    
    try {
        const res = await fetch('/scan');
        const networks = await res.json();
        
        let html = '<ul style="list-style:none;padding:0">';
        networks.forEach(net => {
            html += `<li style="padding:0.5rem;border-bottom:1px solid var(--pico-muted-border-color);cursor:pointer" onclick="selectSSID('${net.ssid}')">
                ${net.ssid} (${net.rssi}dBm) ${net.encrypted ? '🔒' : '🔓'}
            </li>`;
        });
        html += '</ul>';
        
        document.getElementById('networks').innerHTML = html;
        showStatus(`✅ Найдено ${networks.length} сетей`, 'success');
    } catch (e) {
        showStatus('❌ Ошибка сканирования', 'error');
    } finally {
        btn.textContent = '🔍 Сканировать сети';
        btn.disabled = false;
    }
});

function selectSSID(ssid) {
    document.getElementById('ssid').value = ssid;
}

// Подключение к WiFi
document.getElementById('wifiForm').addEventListener('submit', (e) => {
    e.preventDefault();
    
    const data = new URLSearchParams({
        ssid: document.getElementById('ssid').value,
        password: document.getElementById('password').value
    });
    
    showStatus('⏳ Подключение...', 'info');
    
    fetch('/connect', { method: 'POST', body: data })
        .then(res => res.text())
        .then(html => {
            document.open();
            document.write(html);
            document.close();
        });
});

function showStatus(msg, type) {
    const el = document.getElementById('status');
    el.textContent = msg;
    el.className = type;
    el.style.display = 'block';
    setTimeout(() => { 
        el.style.display = 'none';
        el.textContent = '';
    }, 3000);
}

// Интервалы обновления: каждые 5 секунд
setInterval(updateSensors, 5000);
setInterval(updateCalibration, 5000);
updateSensors();
updateCalibration();
)rawliteral";

#endif