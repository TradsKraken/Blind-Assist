#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

// --- Pin Definitions ---
// DFPlayer Mini (Hardware Serial 2)
#define RX2_PIN 16
#define TX2_PIN 17

HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  
  // Initialize DFPlayer
  mySoftwareSerial.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  Serial.println(F("\n--- DFPlayer Debugging Mode ---"));
  Serial.println(F("Initializing DFPlayer..."));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please recheck the connection (RX to TX, TX to RX)!"));
    Serial.println(F("2. Please insert the SD card!"));
    while(true); // Halt here if it fails, since this is a dedicated debug file
  }
  
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(20);  // Set volume value (0~30). Adjust this if it's too loud/quiet.
  
  Serial.println(F("======================================="));
  Serial.println(F("Type a number (1-9) in the Serial Monitor"));
  Serial.println(F("and press Enter to play that track."));
  Serial.println(F("======================================="));
}

void loop() {
  // Listen for input from the Serial Monitor
  if (Serial.available()) {
    char c = Serial.read();
    
    // Check if the user typed a number between 1 and 9
    if (c >= '1' && c <= '9') {
      int trackNumber = c - '0';
      Serial.printf("Command received: Playing track %d...\n", trackNumber);
      myDFPlayer.play(trackNumber);
    }
  }
}
