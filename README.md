# BinGo

> A complete autonomous rover system with line-following navigation and dual-mode remote control interface

<p align="center">
  <img src="https://img.shields.io/badge/Arduino-Motor%20Controller-blue" alt="Arduino">
  <img src="https://img.shields.io/badge/ESP32-WiFi%20Proxy-green" alt="ESP32">
  <img src="https://img.shields.io/badge/Web-Control%20Interface-purple" alt="Web Interface">
</p>

BinGo is a complete IoT rover system that combines embedded hardware control with a modern web-based command interface. The rover autonomously follows line-marked paths while accepting checkpoint navigation commands via WiFi or Bluetooth, making it ideal for autonomous delivery, waste collection, or educational robotics projects.

## 🤖 System Overview

BinGo consists of three integrated components:

- **Arduino Controller** — Main rover logic: motor control, line-following sensors, checkpoint navigation, and command queue management
- **ESP32 Bridge** — WiFi connectivity layer that bridges web commands to the Arduino via serial communication
- **Web Interface** — Modern control dashboard supporting both local WiFi and mobile Bluetooth modes

## ✨ Features

- **Autonomous Line Following** — Sensors track marked paths; motor control adjusts in real-time
- **Checkpoint-Based Navigation** — Queue system manages multiple navigation commands with priority handling
- **Dual Control Modes** — Send commands via WiFi (local network) or Bluetooth (mobile)
- **Real-time Status** — Activity logs and sensor feedback from the rover
- **Modern Dashboard UI** — Glassmorphism design with responsive layout (desktop & mobile)
- **Extensible Architecture** — Modular code across firmware and frontend layers

## 📁 Project Structure

```
BinGo/
├── arduino-esp_code/
│   ├── arduino_code.ino         # Main rover controller
│   │   ├── Motor control (L/R PWM)
│   │   ├── Line-following logic (3 IR sensors)
│   │   ├── Checkpoint queue management
│   │   ├── Command processing from ESP32
│   │   └── Station arrival detection
│   │
│   └── esp32_code.ino           # WiFi bridge to Arduino
│       ├── WiFi connection & IP assignment
│       ├── HTTP POST endpoint for commands
│       ├── Serial forwarding to Arduino
│       └── Command parsing (JSON)
│
└── web/
    ├── index.html               # Home page — mode selection
    ├── css/
    │   ├── main.css             # Home page styles
    │   ├── wifi.css             # WiFi control panel styles
    │   └── bluetooth.css        # Bluetooth control panel styles
    ├── js/
    │   ├── wifi.js              # WiFi connection & HTTP commands
    │   └── bluetooth.js         # Web Bluetooth (GATT) implementation
    ├── wifi/
    │   └── index.html           # WiFi control panel
    ├── bluetooth/
    │   └── index.html           # Bluetooth control panel
    └── server/
        ├── server.js            # Node.js proxy (for local testing)
        └── package.json         # Dependencies
```

## ⚙️ Hardware Setup

### Required Components
- **Arduino Uno/Mega** — Motor control and sensor processing
- **ESP32 DevKit** — WiFi bridge and command handling
- **2x DC Motors** — Rover propulsion (with encoders/PWM control)
- **3x IR Line Sensors** — Left, center, right (digital or analog)
- **Motor Driver** — L298N or similar (2-channel PWM)
- **Ultrasonic Sensor** (optional) — Obstacle detection
- **Power Supply** — Suitable for motors + microcontrollers

### Pin Configuration

**Arduino:**
- Motor pins: `ENA`, `MLB`, `MLF`, `ENB`, `MRB`, `MRF` (configured in code)
- Sensors: `LEFT_SENSOR`, `CENTER_SENSOR`, `RIGHT_SENSOR`
- Serial: UART for ESP32 communication (9600 baud)

**ESP32:**
- Serial1: GPIO 16 (RX), GPIO 17 (TX) → connects to Arduino
- WiFi: Built-in, configured in code

## 🚀 Quick Start

### 1. Upload Firmware

**Arduino:**
```bash
1. Open arduino-esp_code/arduino_code.ino in Arduino IDE
2. Configure your motor and sensor pins
3. Upload to your Arduino board
```

**ESP32:**
```bash
1. Open arduino-esp_code/esp32_code.ino in Arduino IDE (with ESP32 board support)
2. Set WiFi credentials (ssid/password)
3. Configure ARDUINO_RX and ARDUINO_TX pins if needed
4. Upload to your ESP32
```

### 2. Connect Hardware

- Wire Arduino to motors and sensors per your pin configuration
- Connect ESP32 to Arduino via serial (UART1 with logic level shifter if needed)
- Power both boards

### 3. Set Up Web Interface (Option A: Local Server)

```bash
cd web/server
npm install
```

Edit `server.js` with your ESP32's IP address:
```javascript
const ESP32_HOST = 'YOUR_ESP32_IP';
const ESP32_PORT = 80;
```

Start the proxy server:
```bash
npm start
# Server runs on http://localhost:3000
```

### 4. Set Up Web Interface (Option B: Direct Connection)

Skip the Node.js server and open `web/index.html` directly in a browser on the same network as your ESP32 (the ESP32 will need CORS headers configured).

### 5. Control Your Rover

**WiFi Mode:**
1. Open http://localhost:3000 (or http://YOUR_ESP32_IP)
2. Click "WiFi Mode"
3. Select a checkpoint (A, B, C) to queue a navigation command
4. The rover processes the queue and navigates autonomously

**Bluetooth Mode:**
1. Open http://localhost:3000
2. Click "Bluetooth Mode"
3. Click "Connect to Rover" and select your ESP32 from the pairing dialog
4. Send checkpoint commands via Bluetooth

## 📡 Communication Protocol

### Arduino ↔ ESP32
- **Protocol:** Serial UART (9600 baud)
- **Format:** Single character commands (`'0'`, `'1'`, `'2'`, etc. for checkpoints)
- **Direction:** ESP32 sends commands; Arduino responds with status

### Web Interface ↔ ESP32
- **Protocol:** HTTP POST (WiFi) or Bluetooth GATT (mobile)
- **Format:** JSON: `{"command": "0"}` (WiFi) or raw bytes (Bluetooth)
- **Endpoint:** `POST http://ESP32_IP/command`

### Arduino Navigation Logic
- **Line Following:** Reads 3 sensors, adjusts motor PWM in real-time
  - Center sensor: Move forward
  - Left sensor: Correct right (soft right)
  - Right sensor: Correct left (soft left)
- **Checkpoint Queue:** Processes navigation destinations in FIFO order
- **Station Arrival:** Ultrasonic sensor detects checkpoints

## 🛠️ Technologies

| Layer | Technology | Notes |
|-------|-----------|-------|
| **Rover Controller** | Arduino C++ | Motor control, sensors, navigation logic |
| **WiFi Bridge** | ESP32 C++ | HTTP server, serial forwarding |
| **Web Frontend** | HTML5, CSS3, Vanilla JS | No build step required |
| **Proxy Server** | Node.js | Optional, for local testing |
| **Communication** | HTTP, Serial UART, Web Bluetooth | Multiple connectivity options |
| **Hardware** | Arduino, ESP32, DC Motors, IR Sensors | Fully open-source friendly |

## 🎮 Usage Examples

### Send a navigation command via WiFi
```bash
curl -X POST http://ESP32_IP/command \
  -H "Content-Type: application/json" \
  -d '{"command":"1"}'
```

### Queue multiple checkpoints
1. Click checkpoint A (added to rover queue)
2. Click checkpoint B (queued after A)
3. Click checkpoint C (queued after B)
4. Rover navigates A → B → C in sequence

## 🔧 Customization

- **Motor Speed:** Adjust `PWM_MAX` and `PWM_MIN` in arduino_code.ino
- **Sensor Sensitivity:** Tune threshold values in line-following logic
- **UI Theme:** Edit CSS files in `web/css/`
- **WiFi SSID/Password:** Update credentials in esp32_code.ino
- **Checkpoint Behavior:** Modify `arriveAtCheckpoint()` and `arriveAtStation()` in arduino_code.ino

## 📚 How It Works

1. **Power On:** Arduino initializes motors and sensors; ESP32 connects to WiFi
2. **User Sends Command:** Web interface sends checkpoint number to ESP32 via HTTP
3. **Forwarding:** ESP32 receives command, parses JSON, sends character to Arduino via serial
4. **Queue Management:** Arduino enqueues the checkpoint and sets `isRunning = true`
5. **Navigation:** Arduino's main loop continuously reads sensors and adjusts motor PWM
6. **Arrival Detection:** Ultrasonic sensor or timeout confirms checkpoint arrival
7. **Dequeue & Repeat:** Arduino processes next checkpoint in queue

## 🐛 Debugging

**Arduino Serial Monitor:** Set to 9600 baud to see rover status and command logs

**ESP32 Serial Monitor:** Set to 115200 baud to see WiFi connection and HTTP requests

**Browser Console:** Check JavaScript errors and network requests in the web interface

## 👥 Contributors

- [@CrimsonKarma44](https://github.com/CrimsonKarma44) — Project lead, web interface & ESP32 firmware
- [ahmad nakore](mailto:nakoresmacintel@122222222.local) — Arduino controller & navigation logic

## 📄 License

MIT

---

**Made with ❤️ for autonomous robotics** — Contributions welcome!
