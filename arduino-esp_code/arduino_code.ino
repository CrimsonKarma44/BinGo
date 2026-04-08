// ============================================================================
// Project BinGo — 3 IR Sensor Line Follower with WiFi Control
// ============================================================================
// Sensors: LEFT (A0) | CENTER (A1) | RIGHT (A2)
// LOW = white floor detected | HIGH = black line detected
// ============================================================================

#include <SoftwareSerial.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// IR Sensors
#define LEFT_SENSOR     A0
#define CENTER_SENSOR   A1
#define RIGHT_SENSOR    A2

// L298N Motor Driver
#define ENA             6   // PWM — Left motor speed
#define MLF             8   // Left motor forward
#define MLB             9   // Left motor backward
#define ENB             4  // PWM — Right motor speed
#define MRF             3  // Right motor forward
#define MRB             13  // Right motor backward

// ESP32 Communication
#define ESP_RX          1  // ESP32 (RX) -> Arduino TX (D1)
#define ESP_TX          0  // ESP32 (TX) -> Arduino RX (D0)

// User Interaction
#define ULTRASONIC_TRIG 5
#define ULTRASONIC_ECHO 7

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

// Speed Settings (0–255)
#define FULL_SPEED      100
#define TURN_SPEED      50
#define SLOW_SPEED      30
#define STOP_SPEED      0

// Timing
#define DWELL_TIME      5000  // ms to wait at station for trash
#define EXIT_MOVE_MS    200       // ms forward after checkpoint

// System Constants
#define NO_OF_CHECKPOINTS 4
#define MAX_QUEUE       10

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Hardware Objects
// SoftwareSerial espSerial(ESP_RX, ESP_TX);

// Navigation State
bool isRunning = false;
int currentCheckpoint = -1;
bool wasOnCheckpoint = false;

// Queue State
int queue[MAX_QUEUE];
int front = 0;
int rear = 0;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// Motor Control
void moveForward();
void softLeft();
void softRight();
void hardLeft();
void hardRight();
void searchLine();
void stopMotors();

// Station & Checkpoint
void arriveAtStation();
void arriveAtCheckpoint();
void checkQueue();

// Queue Operations
void enqueue(int value);
int dequeue();
int peek();

// ESP32 Communication
int readCommandFromESP();
void executeCommand(String command);

// Ultrasonic Sensor
long readUltrasonicDistance(int triggerPin, int echoPin);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Serial Debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo/Micro, safe for Uno
  }

  // ESP32 Communication
  // espSerial.begin(115200);

  // Sensor Inputs
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(CENTER_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  // Motor Driver Outputs
  pinMode(ENA, OUTPUT);
  pinMode(MLF, OUTPUT);
  pinMode(MLB, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(MRF, OUTPUT);
  pinMode(MRB, OUTPUT);

  // Initialize Motors
  stopMotors();

  Serial.println("BinGo Ready");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Process incoming WiFi commands
  processCommands(); // read from ESP32

  // int cmd = readCommandFromESP();
  
  // if (cmd != -1) {
  //   executeCommand(cmd);
  // }

  // Debug Output
  Serial.println(isRunning ? "Running" : "Not Running");
// Navigation Logic
  if (isRunning) {
    navigateLine();
  }

  delay(20); // Small delay for stability
}

// ============================================================================
// ESP DATA PROCESSING
// ============================================================================
int readCommandFromESP() {
  if (!Serial.available()) return -1; // return -1 if nothing received

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  if (cmd.length() <= 0) return -1;

  int cmdInt = charToInt(cmd[0]);

  Serial.print("Command received: ");
  Serial.println(cmdInt);

  return cmdInt;
}

void processCommands() {
  if (!Serial.available()) return; // return void if nothing received
  
  char cmd = Serial.read();

  // if (cmd == 'S') {
  //   isRunning = true;
  //   Serial.println("Start");
  // } 
  // else if (cmd == 'X') {
  //   isRunning = false;
  //   stopMotors();
  //   Serial.println("Stop");
  // } 
  Serial.print("commend: ");
  Serial.println(cmd);
  if (cmd >= '0' && cmd < ('0' + NO_OF_CHECKPOINTS)) {
    int val = cmd - '0';
    enqueue(val);
    isRunning = true;
  } 
  else {
    Serial.print("Unknown: ");
    Serial.println(cmd);
  }
}

// Char to int
int charToInt(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  return -1; // invalid input
}


// ============================================================================
// NAVIGATION LOGIC
// ============================================================================
void navigateLine() {
  int L = digitalRead(LEFT_SENSOR);
  int C = digitalRead(CENTER_SENSOR);
  int R = digitalRead(RIGHT_SENSOR);

  // Debug Sensor Readings
  Serial.print("L:"); Serial.print(L);
  Serial.print(" C:"); Serial.print(C);
  Serial.print(" R:"); Serial.println(R);

  // Checkpoint Detection: all three sensors on black (HIGH)
  bool isOnCheckpoint = (L == HIGH && C == HIGH && R == HIGH);

  if (isOnCheckpoint && !wasOnCheckpoint) {
    handleCheckpoint();
  }
  // Centered on line: only center sees black
  else if (L == LOW && C == HIGH && R == LOW) {
    moveForward();
  }
  // Soft drift right: center + right on black
  else if (L == LOW && C == HIGH && R == HIGH) {
    softLeft();
  }
  // Soft drift left: left + center on black
  else if (L == HIGH && C == HIGH && R == LOW) {
    softRight();
  }
  // Hard drift right: only right on black
  else if (L == LOW && C == LOW && R == HIGH) {
    hardLeft();
  }
  // Hard drift left: only left on black
  else if (L == HIGH && C == LOW && R == LOW) {
    hardRight();
  }
  // Lost — all white: slow search
  else if (L == LOW && C == LOW && R == LOW) {
    searchLine();
  }
  // Fallback: slow search
  else {
    searchLine();
  }

  wasOnCheckpoint = isOnCheckpoint;
}

// ============================================================================
// CHECKPOINT HANDLING
// ============================================================================
void handleCheckpoint() {
  currentCheckpoint = (currentCheckpoint + 1) % NO_OF_CHECKPOINTS;
  Serial.print("Reached checkpoint: ");
  Serial.println(currentCheckpoint);

  // Stop at station if queue is empty
  if (currentCheckpoint == 0 && front == rear) {
    arriveAtStation();
  } else {
    checkQueue();
  }
}

// ============================================================================
// QUEUE OPERATIONS
// ============================================================================
void enqueue(int value) {
  if ((rear + 1) % MAX_QUEUE == front) {
    Serial.println("Queue Full!");
    return;
  }

  queue[rear] = value;
  rear = (rear + 1) % MAX_QUEUE;

  

  Serial.print("Added checkpoint: ");
  Serial.println(value);
}

int dequeue() {
  if (front == rear) {
    Serial.println("Queue Empty!");
    return -1;
  }

  int value = queue[front];
  front = (front + 1) % MAX_QUEUE;
  return value;
}

int peek() {
  if (front == rear) return -1;
  return queue[front];
}

void checkQueue() {
  int target = peek();
  if (target == -1) return;
  if (currentCheckpoint == target) {
    Serial.print("Arrived at target: ");
    Serial.println(target);
    stopMotors();
    arriveAtCheckpoint();
    dequeue();
  }
}
// ============================================================================
// MOTOR CONTROL FUNCTIONS
// ============================================================================
void moveForward() {
  analogWrite(ENA, FULL_SPEED);
  analogWrite(ENB, FULL_SPEED);
  digitalWrite(MLF, HIGH); digitalWrite(MLB, LOW);
  digitalWrite(MRF, HIGH); digitalWrite(MRB, LOW);
}

void softRight() {
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, SLOW_SPEED);
  digitalWrite(MLF, HIGH); digitalWrite(MLB, LOW);
  digitalWrite(MRF, HIGH); digitalWrite(MRB, LOW);
}

void softLeft() {
  analogWrite(ENA, SLOW_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(MLF, HIGH); digitalWrite(MLB, LOW);
  digitalWrite(MRF, HIGH); digitalWrite(MRB, LOW);
}

void hardRight() {
  analogWrite(ENA, FULL_SPEED);
  analogWrite(ENB, STOP_SPEED);
  digitalWrite(MLF, HIGH); digitalWrite(MLB, LOW);
  digitalWrite(MRF, LOW); digitalWrite(MRB, LOW);
}

void hardLeft() {
  analogWrite(ENA, STOP_SPEED);
  analogWrite(ENB, FULL_SPEED);
  digitalWrite(MLF, LOW); digitalWrite(MLB, LOW);
  digitalWrite(MRF, HIGH); digitalWrite(MRB, LOW);
}

void searchLine() {
  analogWrite(ENA, SLOW_SPEED);
  analogWrite(ENB, SLOW_SPEED);
  digitalWrite(MLF, HIGH); digitalWrite(MLB, LOW);
  digitalWrite(MRF, HIGH); digitalWrite(MRB, LOW);
  Serial.println("Searching line");
}

void stopMotors() {
  analogWrite(ENA, STOP_SPEED);
  analogWrite(ENB, STOP_SPEED);
  digitalWrite(MLF, LOW); digitalWrite(MLB, LOW);
  digitalWrite(MRF, LOW); digitalWrite(MRB, LOW);
}

// ============================================================================
// STATION & CHECKPOINT SEQUENCES
// ============================================================================
void arriveAtStation() {
  Serial.println("Station reached!");
  isRunning = false;
  currentCheckpoint = -1;
  // Clear queue
  front = rear = 0;
  Serial.println("Done. BinGo idle. Press button to dispatch again.");
}

void arriveAtCheckpoint() {
  Serial.println("Checkpoint reached! Waiting for trash...");
  delay(DWELL_TIME);
  Serial.println("Dwell finished. Exiting checkpoint...");
  // Move forward a bit to leave the black area
  moveForward();
  delay(EXIT_MOVE_MS);
  stopMotors();
  Serial.println("Done. Moving to the next checkpoint in the queue...");
}