#include <Arduino.h>
#include <NewPing.h>
#include <DFRobotDFPlayerMini.h>

// --- Pin Definitions ---
// Ultrasonic Sensors
#define TRIG_FRONT 12
#define ECHO_FRONT 14
#define TRIG_LEFT  27
#define ECHO_LEFT  26
#define TRIG_RIGHT 25
#define ECHO_RIGHT 33

// Feedback Modules
#define VIB_MOTOR_PIN 4
#define BUZZER_PIN 5
#define TACTILE_SW_PIN 15

// DFPlayer Mini (Hardware Serial 2)
#define RX2_PIN 16
#define TX2_PIN 17

// --- Constants ---
#define MAX_DISTANCE 400 // Maximum distance in cm (13 feet is approx 396 cm)
#define DANGER_THRESHOLD 100 // cm
#define WARNING_THRESHOLD 250 // cm

// --- Objects ---
NewPing sonarFront(TRIG_FRONT, ECHO_FRONT, MAX_DISTANCE);
NewPing sonarLeft(TRIG_LEFT, ECHO_LEFT, MAX_DISTANCE);
NewPing sonarRight(TRIG_RIGHT, ECHO_RIGHT, MAX_DISTANCE);

HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

// --- State Variables ---
unsigned long lastPingTime = 0;
const unsigned long PING_INTERVAL = 100; // ms

void setup() {
  Serial.begin(115200);
  
  // Initialize feedback pins
  pinMode(VIB_MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(VIB_MOTOR_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Initialize Switch
  pinMode(TACTILE_SW_PIN, INPUT_PULLUP);

  // Initialize DFPlayer
  mySoftwareSerial.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  Serial.println(F("Initializing DFPlayer..."));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    // Don't halt, just continue (for testing without DFPlayer)
  } else {
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(20); // Set volume value (0~30)
  }
}

void playVoiceCommand(int trackNumber) {
  // Assuming track 1: "Obstacle ahead"
  // Track 2: "Move left"
  // Track 3: "Move right"
  myDFPlayer.play(trackNumber);
}

void loop() {
  // Non-blocking timer for sensor reads
  if (millis() - lastPingTime >= PING_INTERVAL) {
    lastPingTime = millis();
    
    // Read distances
    int distFront = sonarFront.ping_cm();
    int distLeft = sonarLeft.ping_cm();
    int distRight = sonarRight.ping_cm();
    
    // Convert 0 (out of range) to max distance
    if (distFront == 0) distFront = MAX_DISTANCE;
    if (distLeft == 0) distLeft = MAX_DISTANCE;
    if (distRight == 0) distRight = MAX_DISTANCE;

    Serial.printf("F:%d L:%d R:%d\n", distFront, distLeft, distRight);

    // Hazard Avoidance Logic
    bool danger = false;
    
    if (distFront < DANGER_THRESHOLD) {
      danger = true;
      if (distLeft > distRight) {
        // More space on the left
        playVoiceCommand(2); // "Move left"
      } else {
        // More space on the right
        playVoiceCommand(3); // "Move right"
      }
    } else if (distLeft < DANGER_THRESHOLD) {
      danger = true;
      playVoiceCommand(3); // "Move right"
    } else if (distRight < DANGER_THRESHOLD) {
      danger = true;
      playVoiceCommand(2); // "Move left"
    }
    
    if (danger) {
      digitalWrite(VIB_MOTOR_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200); // Short pulse
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(VIB_MOTOR_PIN, LOW);
    }
  }

  // Handle Switch (e.g., mute or repeat last command)
  if (digitalRead(TACTILE_SW_PIN) == LOW) {
    delay(50); // Debounce
    if (digitalRead(TACTILE_SW_PIN) == LOW) {
      Serial.println("Switch pressed");
      // Add switch logic here
      while(digitalRead(TACTILE_SW_PIN) == LOW); // Wait for release
    }
  }
}
