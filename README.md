# BinGo

> A complete autonomous rover system with line-following navigation and dual-mode remote control interface

<p align="center">
  <img src="https://img.shields.io/badge/Arduino-Motor%20Controller-blue" alt="Arduino">
  <img src="https://img.shields.io/badge/ESP8266-WiFi%20Module-green" alt="ESP8266">
  <img src="https://img.shields.io/badge/Web-Control%20Interface-purple" alt="Web Interface">
</p>

BinGo is a complete IoT rover system that combines embedded hardware control with a modern web-based command interface. The rover autonomously follows line-marked paths while accepting checkpoint navigation commands via WiFi or Bluetooth, making it ideal for autonomous delivery, waste collection, or educational robotics projects.

## 🤖 System Overview

BinGo consists of three integrated components:

- **Arduino Controller** — Main rover logic: motor control, line-following sensors, checkpoint navigation, and command queue management
- **ESP8266 WiFi Module** — WiFi connectivity layer that bridges web commands to the Arduino via UART serial communication (APT protocol)
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
│   │   ├── Command processing from ESP8266
│   │   └── Station arrival detection
│   │
│   └── esp8266_code.ino         # WiFi module bridge to Arduino
│       ├── WiFi connection & IP assignment
│       ├── HTTP POST endpoint for commands
│       ├── UART/APT serial forwarding to Arduino
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
- **ESP8266 WiFi Module** — WiFi connectivity bridge (e.g., NodeMCU, Wemos D1 Mini, or standalone ESP8266)
- **2x DC Motors** — Rover propulsion (with encoders/PWM control)
- **3x IR Line Sensors** — Left, center, right (digital or analog)
- **Motor Driver** — L298N or similar (2-channel PWM)
- **Ultrasonic Sensor** (optional) — Obstacle detection
- **Logic Level Shifter** (recommended) — Step down 3.3V ESP8266 TX to 5V-safe Arduino RX (to protect Arduino)
- **Power Supply** — Suitable for motors + microcontrollers (separate supplies recommended)

### Pin Configuration & Wiring

**Arduino:**
- Motor pins: `ENA`, `MLB`, `MLF`, `ENB`, `MRB`, `MRF` (configured in code)
- Sensors: `LEFT_SENSOR`, `CENTER_SENSOR`, `RIGHT_SENSOR`
- RX pin: Receives data from ESP8266 TX (via logic level shifter) — typically RX0 or RX1
- TX pin: Sends data to ESP8266 RX — typically TX0 or TX1

**ESP8266 WiFi Module:**
- TX pin (GPIO1) → Arduino RX (via logic level shifter if needed)
- RX pin (GPIO3) → Arduino TX (direct connection, ESP8266 RX is 5V tolerant)
- Ground: Common ground with Arduino and power supply
- 3.3V: From dedicated 3.3V supply (ESP8266 requires stable 3.3V with sufficient current)
- Built-in WiFi for network connectivity

**UART/APT Connection Details:**
- **Baud Rate:** 9600 baud (standard for Arduino-to-WiFi module communication)
- **Protocol:** UART serial communication using APT (Arduino Protocol for Transmission) handshake
- **Data Format:** Single character commands (`'0'`, `'1'`, `'2'`, etc. for checkpoints)
- **Handshake:** APT protocol ensures reliable delivery with acknowledgment frames

### Why Logic Level Shifting?
The ESP8266 operates at 3.3V logic levels while Arduino typically uses 5V. A logic level shifter protects the ESP8266's RX pin from 5V signals while allowing the lower-voltage ESP8266 TX (3.3V) to be safely read by Arduino (which tolerates 3.3V as logic high).

## 🚀 Quick Start

### 1. Upload Firmware

**Arduino:**
```bash
1. Open arduino-esp_code/arduino_code.ino in Arduino IDE
2. Configure your motor and sensor pins
3. Select appropriate serial port (RX/TX pins for ESP8266 communication)
4. Upload to your Arduino board
```

**ESP8266:**
```bash
1. Install ESP8266 board support in Arduino IDE (via Boards Manager)
2. Open arduino-esp_code/esp8266_code.ino in Arduino IDE
3. Select your ESP8266 board variant (NodeMCU 1.0, Wemos D1 Mini, etc.)
4. Set WiFi SSID and password
5. Configure UART pins for Arduino communication (GPIO1 TX, GPIO3 RX)
6. Upload to your ESP8266 module
```

### 2. Connect Hardware

- **Power:** Connect separate 5V supply to Arduino and separate 3.3V supply to ESP8266
- **Wiring:** Connect Arduino to motors and sensors per your pin configuration
- **UART Connection:** Wire ESP8266 to Arduino via logic level shifter (TX→RX, RX→TX, GND→GND)
- **Common Ground:** Ensure all grounds are connected for proper communication

### 3. Test Serial Communication

Open Arduino Serial Monitor (9600 baud) and verify that both Arduino and ESP8266 are communicating via UART. You should see command acknowledgments and status messages.

### 4. Set Up Web Interface (Option A: Local Server)

```bash
cd web/server
npm install
```

Edit `server.js` with your ESP8266's IP address:
```javascript
const ESP8266_HOST = 'YOUR_ESP8266_IP';
const ESP8266_PORT = 80;
```

Start the proxy server:
```bash
npm start
# Server runs on http://localhost:3000
```

### 5. Set Up Web Interface (Option B: Direct Connection)

Skip the Node.js server and open `web/index.html` directly in a browser on the same network as your ESP8266 (the ESP8266 will need CORS headers configured).

### 6. Control Your Rover

**WiFi Mode:**
1. Open http://localhost:3000 (or http://YOUR_ESP8266_IP)
2. Click "WiFi Mode"
3. Select a checkpoint (A, B, C) to queue a navigation command
4. The rover processes the queue and navigates autonomously

**Bluetooth Mode:**
1. Open http://localhost:3000
2. Click "Bluetooth Mode"
3. Click "Connect to Rover" and select your device from the pairing dialog
4. Send checkpoint commands via Bluetooth

## 📡 Communication Protocol

### Arduino ↔ ESP8266 (UART/APT)
- **Protocol:** UART serial with APT handshake (9600 baud)
- **Format:** Single character commands (`'0'`, `'1'`, `'2'`, etc. for checkpoints) with ACK/NAK frames
- **Direction:** ESP8266 sends commands; Arduino responds with status acknowledgments
- **APT Details:**
  - Commands wrapped in APT frames: `[START][CMD][CHECKSUM][END]`
  - Arduino validates checksum and sends acknowledgment
  - Automatic retry on failed transmission
  - Bidirectional communication for status updates

### Web Interface ↔ ESP8266 (HTTP)
- **Protocol:** HTTP POST (WiFi)
- **Format:** JSON: `{"command": "0"}` (checkpoint identifier)
- **Endpoint:** `POST http://ESP8266_IP/command`
- **Response:** JSON status confirmation from rover

### Arduino Navigation Logic
- **Line Following:** Reads 3 sensors, adjusts motor PWM in real-time
  - Center sensor: Move forward
  - Left sensor: Correct right (soft right)
  - Right sensor: Correct left (soft left)
- **Checkpoint Queue:** Processes navigation destinations in FIFO order
- **Station Arrival:** Ultrasonic sensor or timeout confirms checkpoint arrival
- **APT Confirmation:** Arduino acknowledges each command processed in queue

## 🛠️ Technologies

| Layer | Technology | Notes |
|-------|-----------|-------|
| **Rover Controller** | Arduino C++ | Motor control, sensors, navigation logic |
| **WiFi Bridge** | ESP8266 C++ | HTTP server, UART/APT serial forwarding |
| **Web Frontend** | HTML5, CSS3, Vanilla JS | No build step required |
| **Proxy Server** | Node.js | Optional, for local testing |
| **Communication** | HTTP, UART/APT, Web Bluetooth | Multiple connectivity options |
| **Hardware** | Arduino, ESP8266, DC Motors, IR Sensors | Fully open-source friendly |
| **Logic Conversion** | 3.3V-to-5V Logic Level Shifter | Protects ESP8266 from 5V signals |

## 🎮 Usage Examples

### Send a navigation command via WiFi
```bash
curl -X POST http://ESP8266_IP/command \
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
- **WiFi SSID/Password:** Update credentials in esp8266_code.ino
- **UART Baud Rate:** Change from 9600 to higher rate if needed (ensure both Arduino and ESP8266 match)
- **APT Handshake Timeout:** Adjust retry settings in both firmware files
- **Checkpoint Behavior:** Modify `arriveAtCheckpoint()` and `arriveAtStation()` in arduino_code.ino

## 📚 How It Works

1. **Power On:** Arduino initializes motors and sensors; ESP8266 connects to WiFi and establishes UART link with Arduino
2. **User Sends Command:** Web interface sends checkpoint number to ESP8266 via HTTP POST
3. **HTTP Processing:** ESP8266 receives command, parses JSON, wraps command in APT frame
4. **UART Transmission:** ESP8266 sends APT-wrapped command to Arduino via UART (9600 baud)
5. **Arduino Verification:** Arduino receives APT frame, validates checksum, sends acknowledgment back to ESP8266
6. **Queue Management:** Arduino enqueues the checkpoint and sets `isRunning = true`
7. **Navigation:** Arduino's main loop continuously reads sensors and adjusts motor PWM
8. **Arrival Detection:** Ultrasonic sensor or timeout confirms checkpoint arrival
9. **Dequeue & Repeat:** Arduino processes next checkpoint in queue; cycle repeats

## 🐛 Debugging

**Arduino Serial Monitor:** 
- Set to 9600 baud
- Watch for rover status, command receipts, and APT acknowledgments
- Check motor control output and sensor readings

**ESP8266 Serial Monitor:** 
- Set to 115200 baud (standard ESP8266 debug baud rate)
- Monitor WiFi connection status and RSSI signal strength
- View HTTP requests and UART transmission logs

**Browser Console:** 
- Check JavaScript errors and network requests in the web interface
- Verify JSON payload being sent to ESP8266

**UART Communication:**
- Use a logic analyzer or USB-to-UART adapter to monitor the serial bus between Arduino and ESP8266
- Verify APT frames are being transmitted and acknowledged correctly

## 👥 Contributors

- [@CrimsonKarma44](https://github.com/CrimsonKarma44) — Project lead, web interface & ESP8266 firmware
- [ahmad nakore](mailto:nakoresmacintel@122222222.local) — Arduino controller & navigation logic

## 📄 License

MIT

---

**Made with ❤️ for autonomous robotics** — Contributions welcome!
