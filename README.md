# BinGo

> A modern rover control interface powered by ESP32

<p align="center">
  <img src="https://img.shields.io/badge/ESP32-WiFi%20%7C%20Bluetooth-blue" alt="ESP32">
  <img src="https://img.shields.io/badge/Node.js-Proxy%20Server-green" alt="Node.js">
  <img src="https://img.shields.io/badge/Web-Bluetooth%20API-purple" alt="Web Bluetooth">
</p>

BinGo is a sleek, modern web-based control interface for your ESP32-powered rover. Control your rover via WiFi for local networks or Bluetooth for mobile freedom.

## Features

- **Dual Connectivity** — Control via WiFi (for local network use) or Bluetooth (for anywhere)
- **Modern Dark UI** — Glassmorphism design with smooth animations
- **Real-time Feedback** — Live activity logs and status indicators
- **Responsive** — Works on desktop and mobile devices
- **Checkpoint Navigation** — Send navigation commands to your rover with a single tap

## Project Structure

```
Bingo-Bin/
├── arduino-esp_code/          # ESP32/Arduino firmware
│   └── ...
└── web/
    ├── index.html             # Home — choose connection mode
    ├── css/
    │   ├── main.css           # Home page styles
    │   ├── wifi.css           # WiFi control page styles
    │   └── bluetooth.css      # Bluetooth control page styles
    ├── js/
    │   ├── wifi.js            # WiFi connection & commands
    │   └── bluetooth.js       # Web Bluetooth implementation
    ├── wifi/
    │   └── index.html         # WiFi control panel
    ├── bluetooth/
    │   └── index.html         # Bluetooth control panel
    └── server/
        ├── server.js          # Node.js proxy server
        └── package.json       # Server dependencies
```

## Quick Start

### 1. Server Setup

```bash
cd web/server
npm install
```

### 2. Configure ESP32 Connection

Edit `web/server/server.js`:

```javascript
const ESP32_HOST = 'YOUR_ESP32_IP';
const ESP32_PORT = 80;
```

### 3. Run the Server

```bash
npm start
```

The server proxies commands from the web interface to your ESP32 rover.

### 4. Access the Interface

Open `http://localhost:3000` in your browser, or serve the `web/` folder directly.

## Usage

### WiFi Mode

1. Connect your device to the same network as the ESP32
2. Select **WiFi Mode** from the home screen
3. The interface automatically connects to your rover
4. Tap checkpoints (A, B, C) to send navigation commands

### Bluetooth Mode

1. Select **Bluetooth Mode** from the home screen
2. Click **Connect to Rover** and select your device from the pairing dialog
3. Once connected, use the checkpoint buttons to navigate

> **Note:** Web Bluetooth is supported in Chrome, Edge, and Opera on desktop and Android.

## ESP32 Firmware

The `arduino-esp_code/` folder contains the firmware for your ESP32. Upload it using the Arduino IDE or PlatformIO.

The firmware should listen for:
- HTTP POST requests (WiFi mode)
- Serial/Bluetooth commands (Bluetooth mode)

## Technologies

| Layer | Technology |
|-------|------------|
| Frontend | HTML5, CSS3, Vanilla JS |
| Fonts | Inter (Google Fonts) |
| Backend | Node.js |
| Hardware | ESP32 |
| Protocols | HTTP, Web Bluetooth, GATT |

## License

MIT
