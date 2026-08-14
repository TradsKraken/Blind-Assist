# Blind Assist Navigation Belt: Build Guide

![Wiring Diagram](Blind%20Wiring%20Diagram.png)

This project is a wearable belt bag integrated with ultrasonic sensors designed to assist visually impaired individuals. It uses a spatially-aware navigation engine to provide real-time voice guidance and buzzer alerts to actively steer the user around obstacles.

Follow this step-by-step guide to build and configure the device.

---

## Step 1: Gather Materials

The build requires the following hardware components:

* **ESP32 Microcontroller:** The main processor.
* **3x HC-SR04 Ultrasonic Sensors:** To monitor the Front, Left, and Right views.
* **DFPlayer Mini MP3 Player:** To process and play the voice commands.
* **Active Buzzer:** For instant physical feedback and urgency escalation.
* **Micro SD Card:** To store the voice files (must be formatted to FAT32).
* **Breadboard & Jumper Wires:** For circuit assembly.
* **Wearable Belt Bag or Fanny Pack:** To mount the hardware to the user's waist.

---

## Step 2: Understand the Logic Engine

The system uses a highly advanced 65-state logic engine to interpret the sensor data. Instead of simply beeping when an object is near, it actively calculates the best course of action.

* **4 Distance Zones:** Sensors map objects into four specific zones: `CLEAR`, `WARNING`, `CLOSE`, and `DANGER`. 
* **Hysteresis:** The threshold to enter a danger zone is tighter than the threshold to leave it. This prevents the audio from stuttering or "flickering" if an object hovers on a boundary line.
* **Rate-of-Change Detection:** The system actively monitors how fast the distance is shrinking. Sudden drops in distance (e.g., >15cm in a fraction of a second) trigger an immediate emergency override.
* **Spatial Pattern Recognition:** By analyzing the history of the last three sensor readings, the system can identify complex spatial layouts, such as entering a tight corridor or walking into a dead end.

For the exact technical math and the full truth table, reference the [Logic Table Guide](LOGIC_TABLE.md).

---

## Step 3: Prepare the Audio Files

The navigation engine requires 19 specifically formatted audio tracks to provide precise steering commands (e.g., "Left blocked, move right", or "Corridor, walk straight").

1. Format the Micro SD card to **FAT32**.
2. Create a root folder named exactly `MP3`.
3. Use a Text-to-Speech generator to create the required audio phrases. Keep the generated phrases short to ensure fast reaction times.
4. Name the files starting with sequential 4-digit numbers (e.g., `0001_Stop.mp3`, `0002_Move_Left.mp3`).

Reference the [Audio Files Guide](AUDIO_FILES.md) for the exact list of the 19 required phrases and their file numbers.

---

## Step 4: Assemble the Hardware

Use the `Blind Wiring Diagram.png` referenced at the top of this guide to connect the components to the ESP32.

**Important Wiring Note:** 
Ensure the `BUSY` pin on the DFPlayer Mini is connected to Pin 4 on the ESP32. The firmware actively monitors this pin to check if a voice track is currently playing, preventing audio tracks from overlapping or cutting each other off unnecessarily.

---

## Step 5: Upload the Firmware

The system logic is contained within the `Blind.ino` file.

1. Open `Blind.ino` in the Arduino IDE.
2. Install the required dependencies from the Library Manager:
   * `NewPing` (for the ultrasonic sensors)
   * `DFRobotDFPlayerMini` (for the MP3 module)
3. Select the ESP32 board from the Tools menu.
4. Click Upload.

Once powered on, the ESP32 will output real-time distance metrics, zone calculations, and rate-of-change flags directly to the Serial Monitor for debugging and calibration.
