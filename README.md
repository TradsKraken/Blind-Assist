# Blind Assist Navigation Belt: Build Documentation

![Final Look](assets/images/final%20look%20of%20the%20device%20in%20the%20perfboard.jfif)

This project is a wearable belt bag integrated with ultrasonic sensors designed to assist visually impaired individuals. It uses a spatially-aware navigation engine to provide real-time voice guidance and buzzer alerts to actively steer the user around obstacles.

Follow this step-by-step guide to build and configure the device.

---

## The Development Process

The creation of this device involved several iterations to solve complex navigation challenges:

1. **Initial Prototyping (The 27-State Table):** The project began with a simple 3-zone system (Danger, Warning, Clear). However, simple proximity beepers proved insufficient for complex navigation (like approaching corners). To solve this, a 27-state truth table was mapped out to ensure the system always knew exactly what audio phrase to trigger based on every possible combination of the three sensors.
2. **Solving Sensor Flicker (Hysteresis):** Early testing revealed a major UX issue: if a user stood exactly on the boundary of a zone, the sensor would flicker between states, causing the audio to spam them endlessly. This was solved by implementing **Hysteresis**, making the threshold to *enter* a danger zone tighter than the threshold to *exit* it.
3. **The Predictive TTI Experiment:** An attempt was made to track the velocity of the user (Time-To-Impact) to predict collisions before they happened. However, the erratic nature of human walking speeds caused false positives. The complex physics math was ultimately scrapped in favor of a much more reliable **Rate-of-Change (Fast Approach)** check.
4. **The Final 65-State Spatial Engine:** The final firmware expanded to 4 zones (adding a `CLOSE` zone for finer granularity), exploding the truth table to 65+ unique states. Spatial pattern recognition was also added by utilizing a ring buffer to detect Dead Ends, Corridors, and Corners.
5. **Hardware Simplification:** Originally, the build included a tactile switch and a vibration motor. As the logic engine and the 5 distinct buzzer patterns (Emergency, SOS, Rapid, Double, Slow) became more refined, the extra hardware was deemed redundant and stripped out, leaving a perfectly streamlined system relying entirely on intelligent voice and buzzer alerts.

---

## Step 1: Gather Materials

The build requires the following hardware components:

* **ESP32 Microcontroller:** The main processor.
* **3x HC-SR04 Ultrasonic Sensors:** To monitor the Front, Left, and Right views.
* **DFPlayer Mini MP3 Player:** To process and play the voice commands.
* **Active Buzzer:** For instant physical feedback and urgency escalation.
* **Micro SD Card:** To store the voice files (must be formatted to FAT32).
* **Breadboard & Jumper Wires:** For circuit assembly.

---

## Step 2: Understand the Logic Engine

The system uses a highly advanced 65-state logic engine to interpret the sensor data. Instead of simply beeping when an object is near, it actively calculates the best course of action.

### 1. Distance Zones (Hysteresis)
Sensors map objects into four specific zones. The threshold to *enter* a danger zone is tighter than the threshold to *leave* it. This "hysteresis" prevents the audio from stuttering or "flickering" if an object hovers on a boundary line.

| Zone | Enter When | Exit When | 
|------|-----------|-----------|
| 🔴 **DANGER** | ≤ 30 cm | > 35 cm | 
| 🟠 **CLOSE** | 31 – 60 cm | > 70 cm | 
| 🟡 **WARNING** | 61 – 120 cm | > 130 cm |
| 🟢 **CLEAR** | > 120 cm | ≤ 60 cm |

### 2. Rate-of-Change Detection
The system actively monitors how fast the distance is shrinking. Sudden drops in distance trigger an immediate emergency override.

| Approach Speed | Definition | Behavior |
|------|-----------|----------------|
| **FAST APPROACH** | Distance drops > 15 cm instantly | Escalate buzzer to EMERGENCY |
| **STATIC / SLOW** | Distance drops < 15 cm | Normal zone-based buzzer |

### 3. Spatial Pattern Recognition
By analyzing the history of the last three sensor readings across the Left, Front, and Right sensors, the system can identify complex spatial layouts:
- **Dead End:** All three sensors read DANGER.
- **Corridor:** Left and Right are DANGER, Front is CLEAR.
- **Corner Approach:** One side is DANGER, Front is WARNING.

### 4. Buzzer Patterns
Instead of a simple flat tone, the active buzzer plays 5 distinct cadences so the user instantly feels the urgency:
- 🆘 **EMERGENCY:** 5 frantic beeps (Fast Approach / Sudden Object)
- 🔴 **SOS:** 3 urgent beeps (Front DANGER)
- 🟠 **RAPID:** 2 fast beeps (Side DANGER)
- 🟡 **DOUBLE:** 2 moderate beeps (CLOSE zone)
- 🟡 **SLOW:** 1 gentle long beep (WARNING zone)
- 🔕 **SILENT:** No beep (CLEAR zone)

For the exact technical math and the full 65-state truth table, reference the [Logic Table Guide](docs/LOGIC_TABLE.md).

---

## Step 3: Prepare the Audio Files

The navigation engine requires 19 specifically formatted audio tracks to provide precise steering commands (e.g., "Left blocked, move right", or "Corridor, walk straight").

1. Format the Micro SD card to **FAT32**.
2. Create a root folder named exactly `MP3`.
3. Use a Text-to-Speech generator to create the required audio phrases. Keep the generated phrases short to ensure fast reaction times.
4. Name the files starting with sequential 4-digit numbers (e.g., `0001_Stop.mp3`, `0002_Move_Left.mp3`).

Reference the [Audio Files Guide](docs/AUDIO_FILES.md) for the exact list of the 19 required phrases and their file numbers.

---

## Step 4: Assemble the Hardware

![Wiring Diagram](assets/images/Blind%20Wiring%20Diagram.png)

Use the `Blind Wiring Diagram.png` directly above to connect the components to the ESP32.

> [!CAUTION]
> **CRITICAL WARNING:** Do NOT reverse the wires when connecting the battery to the perfboard. Reversing the polarity will damage the components. Please strictly follow the wiring shown in the images below.

### Build Process Gallery
Follow these reference images to see how the build progresses from a breadboard prototype to a final soldered perfboard:

* **Breadboard Prototyping:**
  - ![Temporary Wiring](assets/images/Temporary%20wiring%20in%20breadboard.jfif)
    *Doing a temporary wiring test on the breadboard to ensure all components and code function correctly before moving to the permanent perfboard.*
  - ![Final Breadboard](assets/images/Final%20wiring%20on%20the%20breadboard.jfif)
    *The final layout of the breadboard prototype ready for migration.*

* **Perfboard Layout & Planning:**
  - ![Planned Positions](assets/images/planned%20postions%20of%20the%20components%20in%20the%20perfboard.jfif)
    *Planning the physical placement of the ESP32, DFPlayer Mini, and headers on the perfboard to ensure everything fits compactly.*
  - ![Initial Look](assets/images/initial%20look%20of%20perfboard%20after%20the%20planned%20positions.jfif)
    *The initial look of the perfboard after inserting the main components into their planned positions.*

* **Wiring & Soldering:**
  - ![Sensor & Buzzer Wiring](assets/images/color%20wiring%20of%20the%20ultrasonic%20sensor%20and%20buzzer.jfif)
    *Using color-coded wiring for the ultrasonic sensors and buzzer to keep the connections organized and easy to track.*
  - ![Soldered Back](assets/images/back%20of%20perfboard%20soldered.jfif)
    *The back of the perfboard showing the initial solder points bridging the components.*
  - ![Final Back Wiring](assets/images/final%20wiring%20of%20the%20back%20of%20the%20perfboard.jfif)
    *The completed, final wiring on the back of the perfboard.*

* **Power & Final Assembly:**
  - ![Battery Wiring](assets/images/wiring%20of%20the%20battery%20to%20the%20perfboard.jfif)
    *Connecting the battery wires to the perfboard. **DO NOT REVERSE** these wires to avoid damaging the board!*
  - ![Code Upload](assets/images/Picture%20of%20the%20code.jfif)
    *Uploading the final `Blind.ino` code to the ESP32 via USB.*

**Important Wiring Notes:** 
1. **Trigger/Echo Swap:** The latest codebase has swapped the Trigger and Echo pins compared to the original `Blind Wiring Diagram.png`. Always refer to the pin definitions at the very top of `Blind.ino` for the true source of truth (e.g., Front Trigger is now Pin 25, Front Echo is Pin 26).
2. **DFPlayer BUSY Pin:** Ensure the `BUSY` pin on the DFPlayer Mini is connected to Pin 4 on the ESP32. The firmware actively monitors this pin to check if a voice track is currently playing, preventing audio tracks from overlapping or cutting each other off unnecessarily.

---

## Step 5: Upload the Firmware

The system logic is contained entirely within the firmware file.  
👉 **[View the Full Source Code (Blind.ino)](Blind.ino)**

1. Open `Blind.ino` in the Arduino IDE.
2. Install the required dependencies from the Library Manager:
   * `NewPing` (for the ultrasonic sensors)
   * `DFRobotDFPlayerMini` (for the MP3 module)
3. Select the ESP32 board from the Tools menu.
4. Click Upload.

Once powered on, the ESP32 will output real-time distance metrics, zone calculations, and rate-of-change flags directly to the Serial Monitor for debugging and calibration.

---

<p align="center">
  <img src="assets/images/Fortek%20Electronics.png" alt="Fortek Electronics Logo" width="300">
  <br>
  <b>&copy; Fortek Electronics. All Rights Reserved.</b>
</p>
