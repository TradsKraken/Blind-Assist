# Hardware Wiring Guide

This document describes how to connect the electronic components to the ESP32 microcontroller for the Belt Bag project.

## Components Needed

- 1x ESP32 Development Board
- 3x HC-SR04 Ultrasonic Sensors (Left, Front, Right)
- 1x DFPlayer Mini MP3 Player
- 1x Micro SD Card (formatted as FAT32, for DFPlayer)
- 1x 5V Active Buzzer
- 1x Vibration Motor Module
- 1x Tactile Push Button Switch
- Breadboard & Jumper Wires
- Power supply (e.g., USB Power Bank)

---

## Pin Connections (ESP32)

*Note: These are the default pins used in the provided firmware. You can change them in `src/main.cpp` if necessary.*

### 1. HC-SR04 Ultrasonic Sensors

The ESP32 uses 3.3V logic, while the HC-SR04 often requires 5V. Ensure you use a 3.3V compatible HC-SR04, or use a logic level converter or voltage divider on the `ECHO` pins.

**Front Sensor:**
- `VCC` -> ESP32 `VIN` (5V) or `3.3V` depending on sensor version
- `GND` -> ESP32 `GND`
- `TRIG` -> ESP32 `GPIO 12`
- `ECHO` -> ESP32 `GPIO 14`

**Left Sensor:**
- `VCC` -> ESP32 `VIN` / `3.3V`
- `GND` -> ESP32 `GND`
- `TRIG` -> ESP32 `GPIO 27`
- `ECHO` -> ESP32 `GPIO 26`

**Right Sensor:**
- `VCC` -> ESP32 `VIN` / `3.3V`
- `GND` -> ESP32 `GND`
- `TRIG` -> ESP32 `GPIO 25`
- `ECHO` -> ESP32 `GPIO 33`

---

### 2. DFPlayer Mini (Audio Module)

The DFPlayer communicates via UART (Serial). We use Hardware Serial 2 on the ESP32.

- `VCC` -> ESP32 `VIN` (5V is recommended for better audio output)
- `GND` -> ESP32 `GND`
- `RX` -> ESP32 `GPIO 17` (TX2) *(Important: Add a 1k resistor in series here to prevent noise)*
- `TX` -> ESP32 `GPIO 16` (RX2)
- `SPK1` & `SPK2` -> Connect to a small 8-ohm speaker (or use the headphone jack)

*SD Card Structure:* The audio files should be named `0001.mp3`, `0002.mp3`, etc., and placed in a folder named `mp3` in the root of the SD card.

---

### 3. Vibration Module & Buzzer

These act as outputs to provide haptic and auditory feedback.

**Vibration Module:**
- `VCC` -> ESP32 `3.3V`
- `GND` -> ESP32 `GND`
- `IN` / `Signal` -> ESP32 `GPIO 4`

**Buzzer:**
- `VCC` / `+` -> ESP32 `GPIO 5`
- `GND` / `-` -> ESP32 `GND`

---

### 4. Tactile Switch

Used for user input (e.g., to mute/unmute or reset).

- `Pin 1` -> ESP32 `GPIO 15`
- `Pin 2` -> ESP32 `GND`
*(We will use the ESP32's internal pull-up resistor in the code)*

---

## Assembly Tips

1. **Power:** The ESP32 and DFPlayer can consume significant current, especially when playing audio. A dedicated 5V power bank connected to the ESP32's USB port is recommended.
2. **Mounting:** Ensure the ultrasonic sensors are mounted securely at waist level and face clearly left, forward, and right without obstructing each other's view.
3. **Wire Management:** Use zip ties or heat shrink tubing to manage jumper wires, as this is a wearable device and wires can easily come loose during movement.
