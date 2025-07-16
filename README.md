# 🌱 Smart Watering System

An ESP32-based smart irrigation system using:

- 💧 Soil Moisture Sensor
- 🌡️ DHT11 (Temperature & Humidity)
- 🖥️ OLED Display
- 📱 Blynk App for IoT Control
- 📲 Twilio SMS Alerts
- ⚡ Manual/Auto Mode via Buttons

## 🔧 Hardware Used
- NodeMCU ESP32
- Soil Moisture Sensor
- DHT11 Sensor
- SSD1306 OLED
- Relay Module (Water pump)
- Buzzer, Buttons
- Twilio account
- Blynk app

## 📲 Blynk Virtual Pins
- `V1`: Moisture %
- `V2`: Temperature
- `V3`: Humidity
- `V4`: Mode Switch (Auto/Manual)
- `V5`: Relay Control (ON/OFF)

## 🧠 Features
- Automatic irrigation based on moisture %
- Manual mode for relay control
- Twilio alerts when relay turns ON/OFF
- OLED shows temperature, humidity, mode & moisture level
- Buttons for relay and mode switch (AceButton)
- Real-time monitoring with Blynk

## 📸 Optional Files
- `wiring_diagram.png`:  Add circuit diagram here
- `smart plant watering system.pdf`: Full project documentation 

> Built by Rakesh, Preethi, Snehalatha @ NNRGI (2023-24)
