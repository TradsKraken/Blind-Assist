#include <Arduino.h>
#include <NewPing.h>

// --- Pin Definitions ---
// Ultrasonic Sensors
#define TRIG_FRONT 25
#define ECHO_FRONT 26

#define TRIG_LEFT  12
#define ECHO_LEFT  13

#define TRIG_RIGHT 32
#define ECHO_RIGHT 33

// --- Constants ---
#define MAX_DISTANCE 400 // Maximum distance in cm (13 feet is approx 396 cm)

// --- Objects ---
NewPing sonarFront(TRIG_FRONT, ECHO_FRONT, MAX_DISTANCE);
NewPing sonarLeft(TRIG_LEFT, ECHO_LEFT, MAX_DISTANCE);
NewPing sonarRight(TRIG_RIGHT, ECHO_RIGHT, MAX_DISTANCE);

unsigned long lastPingTime = 0;
const unsigned long PING_INTERVAL = 100; // ms

void setup() {
  Serial.begin(115200);
  Serial.println("Ultrasonic Debugging Mode Started");
  Serial.println("---------------------------------");
}

void loop() {
  // Non-blocking timer for sensor reads
  if (millis() - lastPingTime >= PING_INTERVAL) {
    lastPingTime = millis();
    
    // Read distances
    int distFront = sonarFront.ping_cm();
    int distLeft = sonarLeft.ping_cm();
    int distRight = sonarRight.ping_cm();
    
    // Convert 0 (out of range) to max distance for easier logic later if needed
    if (distFront == 0) distFront = MAX_DISTANCE;
    if (distLeft == 0) distLeft = MAX_DISTANCE;
    if (distRight == 0) distRight = MAX_DISTANCE;

    // Print out the data clearly
    Serial.printf("Left: %3d cm  |  Front: %3d cm  |  Right: %3d cm\n", distLeft, distFront, distRight);
  }
}
