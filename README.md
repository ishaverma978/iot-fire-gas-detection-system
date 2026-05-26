# IoT-Based Fire and Gas Leakage Monitoring and Alert System

Real-time safety monitoring system using NodeMCU ESP8266, 
AWS cloud services, and automated multi-channel alerts.

## 🔧 Hardware
- NodeMCU ESP8266 (microcontroller + Wi-Fi)
- MQ-2 Gas Sensor (LPG, methane, CO detection)
- Flame Sensor (fire detection)
- Relay Module (fan for ventilation)
- Buzzer, Green LED, Red LED

## ☁️ Cloud Architecture
NodeMCU → AWS IoT Core (MQTT/SSL) → AWS Lambda (Python)
↓              ↓
AWS CloudWatch      IFTTT Webhook
(Dashboard)     (SMS + Email + App)

## 🎯 Key Features
- **3-Zone Detection:** Safe (<500), Warning (500-649), Danger (≥650)
- **Secure MQTT:** SSL/TLS with X.509 certificate authentication
- **Serverless Processing:** Python Lambda for cloud-side data routing
- **Live Dashboard:** AWS CloudWatch with 10-second refresh
- **Multi-Channel Alerts:** SMS + Email + App within 5-8 seconds
- **Smart Safety Logic:** Fan OFF during flame detection
- **Alert Cooldown:** 5-minute gap prevents notification spam
- **All Clear Notification:** Complete incident open/close logging

## 📊 Test Results
| Test | Condition | Gas Level | Alert | Fan | IFTTT |
|------|-----------|-----------|-------|-----|-------|
| 1 | Normal | 242 | 0 | OFF | None |
| 2 | Warning zone | 537 | 0 | OFF | None |
| 3 | Gas danger | 724 | 1 | ON | SMS + Email |
| 4 | Flame only | 295 | 1 | OFF | SMS + Email |
| 5 | Gas + Flame | 681 | 1 | OFF | SMS + Email |
| 6 | All Clear | 301 | 0 | OFF | All Clear sent |

## 🛠️ Technologies Used
`NodeMCU ESP8266` `Arduino IDE` `Embedded C` `Python`
`AWS IoT Core` `AWS Lambda` `AWS CloudWatch` `MQTT` 
`SSL/TLS` `X.509 Certificates` `IFTTT Webhooks`
`PubSubClient` `ArduinoJson` `BearSSL`

## 📁 Repository Structure
- `/arduino` — NodeMCU firmware code
- `/lambda` — AWS Lambda Python function  
- `/dashboard` — CloudWatch dashboard JSON
- `/images` — Project screenshots

