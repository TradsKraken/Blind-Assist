# Blind Assist Navigation Belt

![Wiring Diagram](Blind%20Wiring%20Diagram.png)
This project is a wearable belt bag integrated with ultrasonic sensors for advanced obstacle detection. It is designed to assist visually impaired individuals in navigating different environments safely and independently. 

The device provides real-time feedback through **voice guidance, vibration, and multi-pattern buzzer alerts** whenever an obstacle is detected, allowing users to respond promptly and avoid potential hazards.

## Key Features

- **180° Obstacle Detection Coverage:** Utilizes three ultrasonic sensors positioned at the front, left, and right sides of the user.
- **Advanced 4-Zone Logic:** Tracks distance across CLEAR, WARNING, CLOSE, and DANGER zones with built-in hysteresis to prevent flickering feedback.
- **Rate-of-Change Detection:** Actively tracks how fast an obstacle is approaching. Sudden obstacles trigger immediate "Emergency" overrides.
- **Spatial Pattern Recognition:** Intelligently detects if the user has walked into a dead end, entered a corridor, or is approaching a corner.
- **Multimodal Feedback:**
  - **Voice Guidance:** 19 distinct, punchy audio instructions (e.g., "Left blocked. Move right", "Corridor. Walk straight", "Dead end").
  - **Buzzer Patterns:** 5 distinct buzzer cadences (Emergency, SOS, Rapid, Double, Slow) so the user instantly feels the severity of the obstacle without waiting for the audio.

## Materials Used

- ESP32 Microcontroller
- 3x HC-SR04 Ultrasonic sensors
- DFPlayer Mini MP3 Player (with FAT32 SD Card)
- Active Buzzer
- Breadboard & Jumper Wires
- Wearable Belt Bag (Physical prototype)

## Documentation

- **[Hardware Wiring & Audio setup](AUDIO_FILES.md)**: Detailed instructions on the 19 required MP3 files.
- **[Comprehensive Logic Table](LOGIC_TABLE.md)**: The complete 65+ state truth table detailing exactly how the device reacts to every possible sensor combination.
- **Firmware source code**: Located in `Blind.ino`.
- **Debug Utilities**: Located in `UltrasonicDebug/` and `DFPlayerDebug/` folders.

## Setup

1. **Hardware Assembly:** Wire the components to the ESP32 pins defined at the top of `Blind.ino`.
2. **Audio Setup:** Load the 19 required audio clips onto the SD card inside an `MP3` folder as detailed in `AUDIO_FILES.md`. 
3. **Firmware Upload:** Use the Arduino IDE to upload `Blind.ino` to the ESP32. Ensure you have the `NewPing` and `DFRobotDFPlayerMini` libraries installed.

## Disclaimer

This is a prototype device intended for research and development purposes to assist visually impaired individuals. It should be used with caution and not as the sole navigation aid.
