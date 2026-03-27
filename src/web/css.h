#ifndef WEB_CSS_H
#define WEB_CSS_H

#include <Arduino.h>

const char STYLE_CSS[] PROGMEM = R"rawliteral(
* {
    box-sizing: border-box;
    margin: 0;
    padding: 0;
}

body {
    font-family: system-ui, -apple-system, "Segoe UI", Roboto, Arial, sans-serif;
    background: #f5f5f5;
    color: #212529;
    line-height: 1.5;
    padding: 1rem;
}

.container {
    max-width: 800px;
    margin: 0 auto;
}

article {
    background: white;
    border-radius: 0.75rem;
    padding: 1.5rem;
    margin-bottom: 1.5rem;
    box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

h1 {
    font-size: 1.8rem;
    margin-bottom: 1rem;
    color: #333;
}

h2 {
    font-size: 1.3rem;
    margin-bottom: 1rem;
    border-bottom: 2px solid #2196F3;
    padding-bottom: 0.5rem;
    color: #555;
}

.grid-2 {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 1rem;
    margin-bottom: 1rem;
}

.sensor-card {
    background: #f8f9fa;
    border-radius: 0.5rem;
    padding: 1rem;
    text-align: center;
}

.sensor-value {
    font-size: 2rem;
    font-weight: bold;
    color: #2196F3;
}

.sensor-label {
    color: #666;
    font-size: 0.85rem;
    margin-top: 0.25rem;
}

.raw-value {
    font-size: 1.2rem;
    font-weight: bold;
    background: #f0f0f0;
    padding: 0.5rem;
    border-radius: 0.5rem;
    text-align: center;
    font-family: monospace;
}

button, .button {
    background: #2196F3;
    color: white;
    border: none;
    padding: 0.6rem 1rem;
    border-radius: 0.5rem;
    font-size: 0.9rem;
    cursor: pointer;
    transition: background 0.2s;
    width: 100%;
}

button:hover {
    background: #1976D2;
}

button.secondary {
    background: #6c757d;
}

button.secondary:hover {
    background: #5a6268;
}

input, select {
    width: 100%;
    padding: 0.6rem;
    border: 1px solid #ddd;
    border-radius: 0.5rem;
    font-size: 0.9rem;
}

label {
    display: block;
    margin-bottom: 0.3rem;
    font-weight: 500;
    color: #555;
}

hr {
    margin: 1rem 0;
    border: none;
    border-top: 1px solid #eee;
}

details {
    margin-top: 1rem;
    padding: 0.5rem;
    background: #f8f9fa;
    border-radius: 0.5rem;
}

summary {
    cursor: pointer;
    font-weight: 500;
}

#status {
    margin-top: 1rem;
    padding: 0.75rem;
    border-radius: 0.5rem;
    display: none;
}

#status.success {
    background: #d4edda;
    color: #155724;
    display: block;
}

#status.error {
    background: #f8d7da;
    color: #721c24;
    display: block;
}

#status.info {
    background: #d1ecf1;
    color: #0c5460;
    display: block;
}

.network-list {
    border: 1px solid #ddd;
    border-radius: 0.5rem;
    margin: 0.5rem 0;
    max-height: 200px;
    overflow-y: auto;
}

.network-item {
    padding: 0.5rem;
    border-bottom: 1px solid #eee;
    cursor: pointer;
}

.network-item:hover {
    background: #f0f0f0;
}

.text-muted {
    color: #666;
    font-size: 0.75rem;
    margin-top: 0.25rem;
    display: block;
}

@media (max-width: 600px) {
    .grid-2 {
        grid-template-columns: 1fr;
    }
    
    .container {
        padding: 0.5rem;
    }
    
    article {
        padding: 1rem;
    }
}
)rawliteral";

#endif