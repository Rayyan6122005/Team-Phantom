# A.R.T.E.M.I.S. v2.0 - Autonomous Radar Thermal Engine Monitoring Illegal Smuggling

## 📖 Project Overview
A.R.T.E.M.I.S. is a low-latency, full-stack sentry defense system designed for real-time perimeter monitoring. It bridges edge-computing microcontrollers with a high-performance web dashboard. The system utilizes an Arduino UNO for ultrasonic radar sweeping and an ESP-32 for real-time TFT threat display and serial communication, pushing live telemetry to a React-based command center.

## 🌍 Global Impact (SDG-15 Alignment)
**A.R.T.E.M.I.S. directly aligns with UN SDG 15: Life on Land.** By deploying autonomous sentries in protected forest sectors, the system provides an early warning infrastructure to combat illegal logging, wildlife poaching, and the smuggling of contraband, directly preserving biodiversity and protecting terrestrial ecosystems.

## ⚙️ Dependencies and Requirements
**Hardware:**
* Arduino UNO
* ESP-32 (Standard)
* ESP32-CAM (For optical feed)
* SG90 Micro Servo
* HC-SR04 Ultrasonic Sensor
* 1.8" ST7735 TFT Display

**Software Stack:**
* **Frontend:** React, Tailwind CSS, Socket.io-client
* **Backend:** Node.js, Express, Socket.io, SerialPort
* **Firmware:** Arduino C++ (Libraries: `TFT_eSPI`, `Servo`)

## 🚀 Setup Instructions
1. **Hardware:** Wire the components as per the schematics. Connect Arduino TX (Pin 1) to ESP-32 RX2 (Pin 16). Ensure a shared common ground.
2. **Firmware:** Flash `node1_arduino_sweeper.ino` to the Arduino. Flash `node2_esp32_commander.ino` to the ESP-32.
3. **Backend:** ```bash
   cd command_center/backend
   npm install
   node server.js
   ```
