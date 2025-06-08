# Proyecto de Comunicaciones A/D - Grupo 2

## 👥 Integrantes
- [Steven Guala]
- [Edison Navarro]

## 🧠 Objetivo
Diseñar un sistema de adquisición y transmisión de señales usando:
- Sensor MQ-2 (gas)
- Sensor HC-SR04 (ultrasonido)
- ESP8266 con cuantización adaptativa
- Envío a InfluxDB y visualización en Grafana
- Alertas por Telegram
- Procesamiento MATLAB

## ⚙️ Componentes
- NodeMCU ESP8266
- Sensor MQ-2
- Sensor HC-SR04
- Software: Arduino IDE, InfluxDB, Grafana, MATLAB

## 🔌 Conexiones

| Sensor    | Pin ESP8266 |
|-----------|-------------|
| MQ-2      | A0          |
| HC-SR04 TRIG | D5        |
| HC-SR04 ECHO | D6 (con divisor resistivo) |
| VCC       | VIN         |
| GND       | GND         |

## 🧪 Funcionalidad
- Cuantización adaptativa del gas
- Envío de datos a InfluxDB
- Alertas por Telegram si hay gas o distancia crítica
- Análisis FFT y SNR en MATLAB


## 📎 Archivos incluidos
- codigo.ino → Código del ESP8266
- esquema.png → Diagrama de conexiones
- README.md → Este archivo
-