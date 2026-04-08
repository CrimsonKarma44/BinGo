# ESP-01 WiFi Module Setup Guide

## Components Needed

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP-01 (ESP8266) | 1 | WiFi communication |
| 1kΩ resistor | 1 | Voltage divider |
| 2kΩ resistor | 1 | Voltage divider (or two 1kΩ in series) |
| Breadboard | 1 | For connections |
| Jumper wires | 6+ | Connections |
| 3.3V power supply | 1 | ESP-01 needs stable 3.3V |

## Voltage Divider Explanation

**Why?** Arduino TX outputs 5V, but ESP-01 RX only accepts 3.3V. The voltage divider reduces 5V to ~3.3V.

**Formula:** Vout = Vin × (R2 / (R1 + R2))
- Vin = 5V (Arduino TX)
- R1 = 1kΩ
- R2 = 2kΩ
- Vout = 5V × (2000 / 3000) = 3.33V ✓

## Wiring Diagram

```
                    VOLTAGE DIVIDER (for Arduino TX → ESP-01 RX)
                    ┌─────────────────────────────────────────┐
                    │                                         │
Arduino Uno       │         1kΩ          ESP-01              │
Pin 3 (TX) ───────┼───────┬───────┬──────┴────── RX          │
                    │       │       │                         │
                    │       │      ┌┴┐                        │
                    │       │      │ │ 2kΩ                    │
                    │       │      └┬┘                        │
                    │       │       │                         │
                    │       └───────┼──────── GND             │
                    │               │                         │
                    └───────────────┴─────────────────────────┘


COMPLETE WIRING:
┌─────────────────────────────────────────────────────────────────┐
│ ESP-01 Pin  │ Wire Color │ Arduino Uno Pin │ Notes             │
├─────────────┼────────────┼─────────────────┼───────────────────┤
│ VCC         │ Red        │ 3.3V            │ DO NOT USE 5V!    │
│ GND         │ Black      │ GND             │ Common ground     │
│ TX          │ Yellow     │ Pin 2           │ Direct connection │
│ RX          │ Orange     │ Pin 3           │ Via voltage div.  │
│ CH_PD       │ Red        │ 3.3V            │ Pull high         │
│ GPIO0       │ -          │ (open)          │ Leave disconnected│
│ GPIO2       │ -          │ (open)          │ Leave disconnected│
│ RST         │ -          │ (open)          │ Leave disconnected│
└─────────────────────────────────────────────────────────────────┘
```

## Breadboard Setup

```
Breadboard View (top view):

     ┌────────────────────────────────────────────────────────┐
     │  Arduino Uno                                          │
     │    TX(3) ────┬─────────────────────────────────────┐  │
     │              │                                     │  │
     │              ├──[1kΩ]──┬─────── ESP-01 RX         │  │
     │              │         │                           │  │
     │              │        [2kΩ]                        │  │
     │              │         │                           │  │
     │    GND ──────┴─────────┴─────── ESP-01 GND         │  │
     │                                                      │  │
     │    Pin 2 ─────────────────────── ESP-01 TX          │  │
     │                                                      │  │
     │    3.3V ──────────────────────── ESP-01 VCC         │  │
     │    3.3V ──────────────────────── ESP-01 CH_PD       │  │
     └────────────────────────────────────────────────────────┘
```

## Step-by-Step Assembly

### 1. Build Voltage Divider First
1. Insert 1kΩ resistor into breadboard
2. Insert 2kΩ resistor next to it
3. Connect them in series (one leg of each touching same row)
4. Connect the middle point to ESP-01 RX
5. Connect the free 2kΩ leg to GND
6. Connect Arduino TX (Pin 3) to free 1kΩ leg

### 2. Connect Power
1. Arduino 3.3V → ESP-01 VCC (red wire)
2. Arduino 3.3V → ESP-01 CH_PD (red wire)
3. Arduino GND → ESP-01 GND (black wire)
4. Arduino GND → Breadboard ground rail

### 3. Connect Data Lines
1. Arduino Pin 2 → ESP-01 TX (yellow wire)
2. Voltage divider output → ESP-01 RX (orange wire)

## Testing

### 1. Upload Code
1. Open `arduino_esp01_receiver.ino` in Arduino IDE
2. Update WiFi credentials:
   ```cpp
   String wifiCmd = "AT+CWJAP=\"YOUR_SSID\",\"YOUR_PASSWORD\"";
   ```
3. Select Board: Arduino Uno
4. Select Port: (your COM port)
5. Upload

### 2. Monitor Output
1. Open Serial Monitor (9600 baud)
2. You should see:
   ```
   === Arduino Rover Control ===
   Initializing ESP-01 WiFi module...
   ✓ ESP-01 responded!
   Connecting to WiFi...
   ✓ WiFi connected!
   ```

### 3. Test Web Interface
1. Note the IP address shown in Serial Monitor
2. Update `js/server.js`:
   ```javascript
   let serverUrl = 'http://<ESP-01-IP>';
   ```
3. Open `wifi/index.html`
4. Click checkpoint buttons

## Troubleshooting

| Problem | Solution |
|---------|----------|
| ESP-01 not responding | Check 3.3V power (not 5V!), verify wiring |
| WiFi won't connect | Check SSID/password, ensure 2.4GHz network |
| Commands not received | Verify voltage divider on TX line |
| ESP-01 resets randomly | Add 10μF capacitor between VCC and GND |
| Garbage in Serial Monitor | Check baud rate (115200 for ESP, 9600 for Serial) |

## Power Notes

⚠️ **ESP-01 Power Requirements:**
- Voltage: 3.0V - 3.6V (3.3V nominal)
- Current: ~70mA during transmission
- Arduino 3.3V pin can supply ~50mA max

**If ESP-01 resets during WiFi transmission:**
- Use external 3.3V regulator (AMS1117-3.3)
- Add 10μF capacitor across VCC and GND (close to ESP-01)
