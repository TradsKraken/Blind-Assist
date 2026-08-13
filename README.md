# Belt Bag with Voice-Guided Navigation

This project is a wearable belt bag integrated with ultrasonic sensors for obstacle detection. It is designed to assist visually impaired individuals in navigating different environments safely and independently. 

The device provides real-time feedback through voice guidance, vibration, and buzzer alerts whenever an obstacle is detected, allowing users to respond promptly and avoid potential hazards.

## Features

- **180° Obstacle Detection Coverage:** Utilizes three ultrasonic sensors positioned at the front, left, and right sides of the belt bag.
- **Extended Range:** Maximum detection range of up to 13 feet.
- **Multimodal Feedback:**
  - **Voice Guidance:** Provides clear audio instructions such as "Obstacle ahead on your left in three steps."
  - **Vibration Alerts:** Haptic feedback for immediate physical warning.
  - **Buzzer:** Auditory warning mechanism.
- **Intelligent Alerts:** Sensors estimate steps to an obstacle and issue directional voice commands ("Move to the left", "Continue straight") only when necessary to minimize unnecessary alerts.

## Materials Used

- ESP32 Microcontroller
- 3x HC-SR04 Ultrasonic sensors
- DFPlayer Mini MP3 Player (with SD Card)
- Vibration Module
- Buzzer
- Tactile Switch
- Breadboard & Jumper Wires
- Wearable Belt Bag (Physical prototype)

## Documentation

- [Hardware & Wiring Guide](docs/HARDWARE.md): Detailed instructions on connecting the components.
- **Firmware source code**: Located in the `src/` directory.

## Setup

1. **Hardware Assembly:** Follow the wiring guide in `docs/HARDWARE.md` to assemble the circuit.
2. **Audio Setup:** Load the required audio clips (e.g., "Obstacle ahead...", "Move to the left") onto the SD card for the DFPlayer Mini. 
3. **Firmware Upload:** Use PlatformIO or Arduino IDE to upload the code located in `src/main.cpp` to the ESP32.

## Disclaimer

This is a prototype device intended for research and development purposes to assist visually impaired individuals. It should be used with caution and not as the sole navigation aid.
