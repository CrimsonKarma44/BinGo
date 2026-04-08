# Project Overview

**control-server** is a web-based rover control panel that supports two connection modes:
- **WiFi/Server** - HTTP server with ESP32/Arduino receiver
- **Bluetooth** - Direct BLE communication with HM-10 module

## Project Structure

```
control-server/
├── index.html          # Landing page with mode selection
├── css/
│   └── styles.css      # Shared styles
├── js/
│   ├── bluetooth.js    # Bluetooth connection logic
│   └── server.js       # WiFi/Server request logic
├── wifi/
│   └── index.html      # WiFi control panel
├── bluetooth/
│   └── index.html      # Bluetooth control panel
└── server/
    ├── server.js       # Node.js HTTP server
    ├── package.json    # Node.js dependencies
    ├── esp32_receiver.ino    # ESP32 WiFi receiver code
    └── arduino_receiver.ino  # Arduino + ESP-01 receiver code
```

## Usage

### WiFi Mode

**1. Start the Node.js server (for testing):**
```bash
cd server
npm install
npm start
```

**2. Open the control panel:**
- Open `wifi/index.html` in a browser
- Click checkpoint buttons to send commands
- Server responds with `OK:<command>` format

**3. ESP32 Receiver:**
- Flash `esp32_receiver.ino` to your ESP32
- Update WiFi credentials in the code
- ESP32 listens on port 80 for `/command` POST requests
- Response format: `OK:1`, `OK:2`, `OK:3`

**4. Arduino + ESP-01 Receiver:**
- Flash `arduino_receiver.ino` to your Arduino
- Connect ESP-01 to pins 2 & 3 (SoftwareSerial)
- ESP-01 should forward received commands via serial

### Bluetooth Mode

1. Open `bluetooth/index.html` in a **Web Bluetooth-compatible browser** (Chrome, Edge, Opera)
2. Click **"Connect to Rover"** to pair with HM-10 BLE device
3. Click checkpoint buttons to send commands

## Server API (WiFi Mode)

| Endpoint | Method | Request Body | Response | Description |
|----------|--------|--------------|----------|-------------|
| `/command` | POST | `{"command": "1"}` | `OK:1` | Send checkpoint command |

**Response format for ESP32/Arduino:**
- Success: `OK:<command>` (e.g., `OK:1`, `OK:2`, `OK:3`)
- Error: `ERROR:<message>` (e.g., `ERROR:Invalid JSON`)

Simple text responses are used for easy parsing on microcontrollers.

## Bluetooth Configuration

- **Service UUID:** `ffe0`
- **Characteristic UUID:** `ffe1`
- **Device Filters:** HM-10, HMSoft, or devices with service `ffe0`

## Browser Compatibility

| Browser | WiFi Mode | Bluetooth Mode |
|---------|-----------|----------------|
| Chrome | ✅ | ✅ |
| Edge | ✅ | ✅ |
| Opera | ✅ | ✅ |
| Firefox | ✅ | ❌ |
| Safari | ✅ | ❌ |
