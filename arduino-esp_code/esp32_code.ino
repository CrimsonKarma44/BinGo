#include <WiFi.h>
#include <WebServer.h>

// ============== WiFi Configuration ==============
// const char* ssid = "Galaxy S21 5G";
// const char* password = "nakore11001";

const char *ssid = "Redmi14C";
const char *password = "passwordll";

// ============== Serial Communication ==============
// Serial0 (RX=GPIO3, TX=GPIO1) is used for debugging
// Serial1 (RX=GPIO9, TX=GPIO10) communicates with Arduino
// If your board uses different pins, change these values
// #define ARDUINO_RX 9   // ESP32 RX pin (receives from Arduino TX)
// #define ARDUINO_TX 10  // ESP32 TX pin (sends to Arduino RX)
#define ARDUINO_RX 16   // ESP32 RX pin (receives from Arduino TX)
#define ARDUINO_TX 17   // ESP32 TX pin (sends to Arduino RX)
#define BAUD_RATE 9600 // Must match Arduino's Serial.begin() rate

HardwareSerial ArduinoSerial(1); // Use UART1 for Arduino communication
WebServer server(80);

// ============== Function Declarations ==============
void connectToWiFi();
void handleCommand();
String sendCommandToArduino(String command);

void setup() {
  // Initialize Debug Serial (USB) at 115200 baud
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== ESP32 Startup ===");
  
  // Initialize Arduino Serial (UART1) at 9600 baud
  ArduinoSerial.begin(BAUD_RATE, SERIAL_8N1, ARDUINO_RX, ARDUINO_TX);
  Serial.println("Arduino serial initialized at 9600 baud");
  
  // Connect to WiFi network
  connectToWiFi();
  
  // Setup HTTP route handler
  // POST /command route expects JSON: {"command": "your_command_here"}
  server.on("/command", HTTP_POST, handleCommand);
  server.begin();
  Serial.println("HTTP server started on port 80");
  Serial.println("Ready to receive commands at http://command");
}

void loop() {
  // Handle incoming HTTP requests from web clients
  server.handleClient();
  
  // Optional: Check for data from Arduino and print to debug console
  if (ArduinoSerial.available()) {
    Serial.print("Arduino available: ");
    String response = ArduinoSerial.readStringUntil('\n');
    Serial.print("Arduino response (unsolicited): ");
    Serial.println(response);
  } else
    Serial.println("Arduino available: No");
  }
  
  delay(10); // Small delay to prevent CPU overuse
}

// ============== WiFi Connection Function ==============
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

// ============== HTTP Command Handler ==============
void handleCommand() {
  // Get the raw request body (JSON data)
  String body = server.arg("plain");
  Serial.print("Received HTTP request body: ");
  Serial.println(body);
  
  // Parse JSON to extract the "command" field
  // Expected format: {"command":"your_command"}
  int start = body.indexOf("\"command\"") + 11;
  int end = body.indexOf("\"", start);
  
  if (start > 10 && end > start) {  // Validate parsing was successful
    String command = body.substring(start, end);
    Serial.print("Parsed command: ");
    Serial.println(command);
    
    // Send command to Arduino and get response
    String response = sendCommandToArduino(command);
    
    // Send HTTP response back to client
    server.send(200, "text/plain", response);
  } else {
    // Invalid JSON format
    Serial.println("Error: Invalid JSON format in request");
    server.send(400, "text/plain", "Error: Invalid JSON format");
  }
}

// ============== Arduino Serial Communication ==============
String sendCommandToArduino(String command) {
  /*
   * Send a command to Arduino and wait for response
   * 
   * How it works:
   * 1. Send the command string over serial to Arduino
   * 2. Add newline so Arduino knows when message ends
   * 3. Wait for Arduino to respond (with timeout)
   * 4. Read the response and return it
   */
  
  Serial.print("Sending to Arduino: ");
  Serial.println(command);
  
  // Send command to Arduino with newline terminator
  ArduinoSerial.println(command);
  
  // Wait for Arduino response with a timeout
  unsigned long startTime = millis();
  unsigned long timeout = 2000;  // Wait max 2 seconds for response
  String response = "";
  
  while (millis() - startTime < timeout) {
    if (ArduinoSerial.available()) {
      char c = ArduinoSerial.read();
      
      // Build response until newline is received
      if (c == '\n') {
        Serial.print("Response from Arduino: ");
        Serial.println(response);
        return "OK:" + response;
      } else if (c != '\r') {  // Ignore carriage returns
        response += c;
      }
    }
    delay(10);
  }
  
  // Timeout: Arduino didn't respond
  Serial.println("Error: No response from Arduino (timeout)");
  return "Error: Arduino timeout";
}