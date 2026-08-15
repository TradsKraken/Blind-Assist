# Comprehensive Obstacle Avoidance Logic Table
### 3 Ultrasonic Sensors (Left, Front, Right) + Buzzer + Voice Audio

---

## 1. Distance Zones (Per Sensor)

| Zone | Enter When | Exit When | Color |
|------|-----------|-----------|-------|
| 🔴 **DANGER** | ≤ 30 cm | > 35 cm | Critical |
| 🟠 **CLOSE** | 31 – 60 cm | > 70 cm | Very Close |
| 🟡 **WARNING** | 61 – 120 cm | > 130 cm | Caution |
| 🟢 **CLEAR** | > 120 cm | ≤ 60 cm | Open |

> Adding a **CLOSE** zone between DANGER and WARNING gives much finer-grained audio feedback, letting the user feel the transition before it becomes a crisis.

---

## 2. Rate-of-Change Detection (Approach Speed)

The firmware should track how fast each sensor's distance is **shrinking** between readings:

| Rate | Definition | Extra Behavior |
|------|-----------|----------------|
| **FAST APPROACH** | Distance drops > 15 cm between two readings | Escalate buzzer pattern one step immediately |
| **SLOW APPROACH** | Distance drops 1–15 cm between two readings | Normal zone-based buzzer |
| **STATIC** | Distance change ≤ 1 cm (likely a wall) | Sustain current buzzer pattern |
| **RECEDING** | Distance increasing | De-escalate buzzer pattern if zone improves |

> **Example:** Sensor reads 90 cm then 60 cm in one cycle → FAST APPROACH. Even though zone is still WARNING, buzzer escalates to RAPID immediately.

---

## 3. Buzzer Patterns

| Pattern | Sequence | Feel | Used For |
|---------|----------|------|----------|
| 🆘 **EMERGENCY** | 5 beeps (30ms ON / 30ms OFF) | Frantic | Sudden object appearing in DANGER zone |
| 🔴 **SOS** | 3 beeps (50ms ON / 50ms OFF) | Urgent | Front DANGER — stop and turn |
| 🟠 **RAPID** | 2 beeps (100ms ON / 100ms OFF) | Fast | Side DANGER while front is clear |
| 🟡 **DOUBLE** | 2 beeps (200ms ON / 300ms OFF) | Moderate | CLOSE zone, warning escalation |
| 🟡 **SLOW** | 1 long beep (300ms ON / 600ms OFF) | Gentle | General WARNING approach |
| 🔕 **SILENT** | No beep | — | CLEAR, path open |

---

## 4. Required Audio Tracks

| Track # | File Name | Spoken Phrase | When Used |
|---------|-----------|---------------|-----------|
| 1 | `0001_Stop.mp3` | *"Stop."* | Fast approach emergency |
| 2 | `0002_Move_Left.mp3` | *"Move left."* | Front blocked, left is clearer |
| 3 | `0003_Move_Right.mp3` | *"Move right."* | Front blocked, right is clearer |
| 4 | `0004_Stop_All_Around.mp3` | *"Stop. Obstacles all around. Please wait."* | Dead end / fully enclosed |
| 5 | `0005_Narrow_Path.mp3` | *"Caution. Narrow path ahead."* | Both sides DANGER, front WARNING |
| 6 | `0006_Caution_Ahead.mp3` | *"Caution. Obstacle ahead."* | Front WARNING |
| 7 | `0007_Narrow_Passage.mp3` | *"Caution. Narrow passage. Move slowly."* | Both sides DANGER, front CLEAR |
| 8 | `0008_Left_Move_Right.mp3` | *"Obstacle on your left. Move right."* | Left DANGER, front CLEAR |
| 9 | `0009_Right_Move_Left.mp3` | *"Obstacle on your right. Move left."* | Right DANGER, front CLEAR |
| 10 | `0010_Path_Clear.mp3` | *"Path clear."* | All zones CLEAR (once only) |
| 11 | `0011_Turn_Left.mp3` | *"Turn left."* | Corner detected on right side |
| 12 | `0012_Turn_Right.mp3` | *"Turn right."* | Corner detected on left side |
| 13 | `0013_Dead_End.mp3` | *"Dead end. Turn around."* | All 3 sensors closing in fast |
| 14 | `0014_Close_Left.mp3` | *"Close on your left."* | Left enters CLOSE zone |
| 15 | `0015_Close_Right.mp3` | *"Close on your right."* | Right enters CLOSE zone |
| 16 | `0016_Slow_Down.mp3` | *"Slow down."* | FAST APPROACH detected |
| 17 | `0017_Corridor.mp3` | *"You are in a corridor. Walk straight."* | Both sides WARNING/CLOSE, front CLEAR |
| 18 | `0018_Step_Left.mp3` | *"Step slightly left."* | Right CLOSE (not yet DANGER), front CLEAR |
| 19 | `0019_Step_Right.mp3` | *"Step slightly right."* | Left CLOSE (not yet DANGER), front CLEAR |

---

## 5. Full Logic Table — All Scenarios

### 🔴 GROUP A — EMERGENCY (Sudden Appearance)
*Object goes from CLEAR to DANGER in 1-2 sensor cycles. Overrides everything.*

| Scenario | Pattern | Audio | Navigation Instruction |
|----------|---------|-------|----------------------|
| Any sensor: CLEAR → DANGER in 1 cycle | 🆘 EMERGENCY | Track 1: *"Stop."* + Track 16: *"Slow down."* | Halt immediately, do not move until clear |

---

### 🔴 GROUP B — FRONT DANGER (Rows 1–9)
*User is about to walk into something directly in front.*

| # | Left | Front | Right | Rate | Buzzer | Audio | Navigation |
|---|------|-------|-------|------|--------|-------|-----------|
| 1 | Clear | 🔴 DANGER | Clear | Any | 🔴 SOS | Track 2 or 3 (compare raw cm) | Turn towards whichever side has more space |
| 2 | Clear | 🔴 DANGER | Warning | Any | 🔴 SOS | Track 2: *"Move left."* | Left is much clearer — turn left |
| 3 | Clear | 🔴 DANGER | Close | Any | 🔴 SOS | Track 2: *"Move left."* | Left is clearly safer |
| 4 | Clear | 🔴 DANGER | 🔴 DANGER | Any | 🔴 SOS | Track 2: *"Move left."* | Only left is free |
| 5 | Warning | 🔴 DANGER | Clear | Any | 🔴 SOS | Track 3: *"Move right."* | Right is much clearer — turn right |
| 6 | Warning | 🔴 DANGER | Warning | Any | 🔴 SOS | Track 2 or 3 (compare raw cm) | Steer toward whichever side reads farther |
| 7 | Warning | 🔴 DANGER | 🟠 Close | Any | 🔴 SOS | Track 2: *"Move left."* | Left is relatively safer |
| 8 | Warning | 🔴 DANGER | 🔴 DANGER | Any | 🔴 SOS | Track 2: *"Move left."* | Only left is free |
| 9 | 🟠 Close | 🔴 DANGER | Clear | Any | 🔴 SOS | Track 3: *"Move right."* | Right clearly open |
| 10 | 🟠 Close | 🔴 DANGER | Warning | Any | 🔴 SOS | Track 3: *"Move right."* | Right is relatively clearer |
| 11 | 🟠 Close | 🔴 DANGER | 🟠 Close | Any | 🔴 SOS | Track 2 or 3 (compare raw cm) | Choose the less close side |
| 12 | 🟠 Close | 🔴 DANGER | 🔴 DANGER | Any | 🔴 SOS | Track 2: *"Move left."* | Left is least dangerous |
| 13 | 🔴 DANGER | 🔴 DANGER | Clear | Any | 🔴 SOS | Track 3: *"Move right."* | Only right is free |
| 14 | 🔴 DANGER | 🔴 DANGER | Warning | Any | 🔴 SOS | Track 3: *"Move right."* | Right is clearly safer |
| 15 | 🔴 DANGER | 🔴 DANGER | 🟠 Close | Any | 🔴 SOS | Track 3: *"Move right."* | Right is relatively safer |
| 16 | 🔴 DANGER | 🔴 DANGER | 🔴 DANGER | Any | 🆘 EMERGENCY | Track 4: *"Stop. Obstacles all around."* | Do not move — full dead end |
| 17 | Any | 🔴 DANGER | Any | FAST | 🆘 EMERGENCY | Track 1: *"Stop."* then Track 16: *"Slow down."* | Emergency stop regardless of sides |

---

### 🟠 GROUP C — FRONT CLOSE (Very Near, Not Yet Danger)
*User has about 30–60 cm before hitting something. Time to slow down.*

| # | Left | Front | Right | Buzzer | Audio | Navigation |
|---|------|-------|-------|--------|-------|-----------|
| 18 | Clear | 🟠 Close | Clear | 🟡 DOUBLE | Track 6: *"Caution. Obstacle ahead."* | Slow down, prepare to turn |
| 19 | Clear | 🟠 Close | Warning | 🟡 DOUBLE | Track 6 | Slow down, slight left bias |
| 20 | Clear | 🟠 Close | 🟠 Close | 🟡 DOUBLE | Track 6 | Slow down and start moving left |
| 21 | Clear | 🟠 Close | 🔴 DANGER | 🔴 SOS | Track 2: *"Move left."* | Begin turning left now |
| 22 | Warning | 🟠 Close | Clear | 🟡 DOUBLE | Track 6 | Slow down, slight right bias |
| 23 | Warning | 🟠 Close | Warning | 🟡 DOUBLE | Track 6 | Slow down, compare sides to choose |
| 24 | Warning | 🟠 Close | 🟠 Close | 🟡 DOUBLE | Track 5: *"Narrow path."* | Very narrow — slow down significantly |
| 25 | Warning | 🟠 Close | 🔴 DANGER | 🔴 SOS | Track 2: *"Move left."* | Turn left while there is still space |
| 26 | 🟠 Close | 🟠 Close | Clear | 🟡 DOUBLE | Track 3: *"Move right."* | Favor moving right away from left |
| 27 | 🟠 Close | 🟠 Close | Warning | 🟡 DOUBLE | Track 3: *"Move right."* | Right is clearer |
| 28 | 🟠 Close | 🟠 Close | 🟠 Close | 🔴 SOS | Track 5: *"Narrow path."* | Very narrow on all sides — move very slowly |
| 29 | 🟠 Close | 🟠 Close | 🔴 DANGER | 🔴 SOS | Track 2: *"Move left."* | Only left is viable |
| 30 | 🔴 DANGER | 🟠 Close | Clear | 🔴 SOS | Track 3: *"Move right."* | Turn right urgently |
| 31 | 🔴 DANGER | 🟠 Close | Warning | 🔴 SOS | Track 3: *"Move right."* | Right is clearly safer |
| 32 | 🔴 DANGER | 🟠 Close | 🟠 Close | 🔴 SOS | Track 5: *"Narrow path."* | Squeezed in from left with front close |
| 33 | 🔴 DANGER | 🟠 Close | 🔴 DANGER | 🆘 EMERGENCY | Track 4: *"Stop. Obstacles all around."* | Stop — almost fully enclosed |

---

### 🟡 GROUP D — FRONT WARNING (Obstacle Approaching)
*User is 60–120 cm from something. Time to course-correct.*

| # | Left | Front | Right | Buzzer | Audio | Navigation |
|---|------|-------|-------|--------|-------|-----------|
| 34 | Clear | 🟡 Warn | Clear | 🟡 SLOW | Track 6: *"Caution. Obstacle ahead."* | Can continue with caution |
| 35 | Clear | 🟡 Warn | Warning | 🟡 SLOW | Track 6 | Slight left bias to be safe |
| 36 | Clear | 🟡 Warn | 🟠 Close | 🟡 DOUBLE | Track 2: *"Move left."* | Start steering left now |
| 37 | Clear | 🟡 Warn | 🔴 DANGER | 🟠 RAPID | Track 2: *"Move left."* | Turn left — right is in danger |
| 38 | Warning | 🟡 Warn | Clear | 🟡 SLOW | Track 6 | Slight right bias to be safe |
| 39 | Warning | 🟡 Warn | Warning | 🟡 SLOW | Track 6 | Slow, compare sides subtly |
| 40 | Warning | 🟡 Warn | 🟠 Close | 🟡 DOUBLE | Track 2: *"Move left."* | Left is safer, start turning |
| 41 | Warning | 🟡 Warn | 🔴 DANGER | 🟠 RAPID | Track 2: *"Move left."* | Turn left now |
| 42 | 🟠 Close | 🟡 Warn | Clear | 🟡 DOUBLE | Track 3: *"Move right."* | Left is very close — steer right |
| 43 | 🟠 Close | 🟡 Warn | Warning | 🟡 DOUBLE | Track 3: *"Move right."* | Right has more space |
| 44 | 🟠 Close | 🟡 Warn | 🟠 Close | 🟡 DOUBLE | Track 17: *"Walk straight. Corridor."* | Both sides close — walk straight carefully |
| 45 | 🟠 Close | 🟡 Warn | 🔴 DANGER | 🔴 SOS | Track 2: *"Move left."* | Left is the only viable option |
| 46 | 🔴 DANGER | 🟡 Warn | Clear | 🟠 RAPID | Track 3: *"Move right."* | Turn right now |
| 47 | 🔴 DANGER | 🟡 Warn | Warning | 🟠 RAPID | Track 3: *"Move right."* | Right is clearly open |
| 48 | 🔴 DANGER | 🟡 Warn | 🟠 Close | 🔴 SOS | Track 5: *"Narrow path."* | Left DANGER + front warning — slow, small right nudge |
| 49 | 🔴 DANGER | 🟡 Warn | 🔴 DANGER | 🔴 SOS | Track 5: *"Narrow path ahead."* | Ultra-narrow — walk dead straight slowly |

---

### 🟢 GROUP E — FRONT CLEAR (Path Open Ahead, React to Sides)
*Lowest priority for front. Mostly side obstacle management.*

| # | Left | Front | Right | Buzzer | Audio | Navigation |
|---|------|-------|-------|--------|-------|-----------|
| 50 | Clear | 🟢 Clear | Clear | 🔕 SILENT | Track 10: *"Path clear."* (once) | Walk freely |
| 51 | Clear | 🟢 Clear | Warning | 🔕 SILENT | Track 10 (once) | Slight left bias — right side approaching |
| 52 | Clear | 🟢 Clear | 🟠 Close | 🟡 SLOW | Track 15: *"Close on your right."* | Move slightly left |
| 53 | Clear | 🟢 Clear | 🔴 DANGER | 🟠 RAPID | Track 9: *"Obstacle on right. Move left."* | Steer left now |
| 54 | Warning | 🟢 Clear | Clear | 🔕 SILENT | Track 10 (once) | Slight right bias |
| 55 | Warning | 🟢 Clear | Warning | 🔕 SILENT | Track 10 (once) | Walk forward, both sides have space |
| 56 | Warning | 🟢 Clear | 🟠 Close | 🟡 SLOW | Track 15: *"Close on your right."* | Nudge slightly left |
| 57 | Warning | 🟢 Clear | 🔴 DANGER | 🟠 RAPID | Track 9: *"Obstacle on right. Move left."* | Move left |
| 58 | 🟠 Close | 🟢 Clear | Clear | 🟡 SLOW | Track 14: *"Close on your left."* | Step slightly right |
| 59 | 🟠 Close | 🟢 Clear | Warning | 🟡 SLOW | Track 19: *"Step slightly right."* | Nudge right gently |
| 60 | 🟠 Close | 🟢 Clear | 🟠 Close | 🟡 SLOW | Track 17: *"Walk straight. Corridor."* | Centered in a corridor — walk straight |
| 61 | 🟠 Close | 🟢 Clear | 🔴 DANGER | 🟠 RAPID | Track 9: *"Obstacle on right. Move left."* | Left is tight but right is in danger — nudge left |
| 62 | 🔴 DANGER | 🟢 Clear | Clear | 🟠 RAPID | Track 8: *"Obstacle on left. Move right."* | Turn right |
| 63 | 🔴 DANGER | 🟢 Clear | Warning | 🟠 RAPID | Track 8: *"Obstacle on left. Move right."* | Move right |
| 64 | 🔴 DANGER | 🟢 Clear | 🟠 Close | 🟠 RAPID | Track 8: *"Obstacle on left. Move right."* | Left DANGER, right is still navigable |
| 65 | 🔴 DANGER | 🟢 Clear | 🔴 DANGER | 🟡 SLOW | Track 7: *"Narrow passage. Move slowly."* | Both sides in danger but front is clear — walk straight |

---

## 6. Special Spatial Pattern Detection

These are *patterns* detected by comparing how sensors change together, not just a snapshot.

| Pattern | How to Detect | Buzzer | Audio | Navigation |
|---------|--------------|--------|-------|-----------|
| **CORNER (Left)** | Left quickly → DANGER while Front transitions from CLEAR → WARNING | 🔴 SOS | Track 12: *"Turn right."* | A left-hand wall is ending — turn right |
| **CORNER (Right)** | Right quickly → DANGER while Front transitions from CLEAR → WARNING | 🔴 SOS | Track 11: *"Turn left."* | A right-hand wall is ending — turn left |
| **DEAD END** | All 3 sensors transition from WARNING → DANGER within 3 consecutive cycles | 🆘 EMERGENCY | Track 13: *"Dead end. Turn around."* | Stop and rotate 180° |
| **CORRIDOR ENTRY** | Both Left and Right enter WARNING at the same time while Front stays CLEAR | 🔕 SILENT | Track 17: *"You are in a corridor. Walk straight."* | Maintain straight heading |
| **CORRIDOR EXIT** | Both Left and Right improve to CLEAR simultaneously while walking | 🔕 SILENT | Track 10: *"Path clear."* | Open space — free to move |
| **DIAGONAL OBSTACLE** | Only Front + one side in DANGER (other side clear) | 🔴 SOS | Track 2 or 3 accordingly | Object is at an angle — turn to the clear side |

---

## 7. Priority & Interrupt Rules

| Priority | Level | Tracks | Can Interrupt |
|----------|-------|--------|---------------|
| **0** | Emergency / Sudden | 1, 13 | Everything, always |
| **1** | Immediate Front Danger | 2, 3, 4 | Priority 1–4 |
| **2** | Side Danger / Squeeze | 5, 7, 8, 9, 11, 12 | Priority 2–4 |
| **3** | Close Zone / Caution | 6, 14, 15, 16, 17 | Priority 3–4 only |
| **4** | Informational | 10, 18, 19 | Never interrupts |

---

## 8. Anti-Spam Timing Rules

| Condition | Timing Behavior |
|-----------|----------------|
| Priority 0 (Emergency) | Speak immediately, interrupt anything |
| Priority 1 (Immediate Danger) | Speak immediately; repeat every **2.5 seconds** while active |
| Priority 2 (Side Danger) | Speak if player idle or higher priority; repeat every **4 seconds** |
| Priority 3 (Caution / Close) | Speak if player idle; repeat every **6 seconds** |
| Priority 4 (Informational) | Speak **once** only on the transition into the state |
| Track 10 ("Path clear") | Speak **once** on state entry — never repeat on timer |

---

## 9. Buzzer Code Reference

```cpp
// EMERGENCY — 5 very fast beeps
void buzzerEmergency() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(30);
    digitalWrite(BUZZER_PIN, LOW);  delay(30);
  }
}

// SOS — 3 fast beeps
void buzzerSOS() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(50);
    digitalWrite(BUZZER_PIN, LOW);  delay(50);
  }
}

// RAPID — 2 fast beeps
void buzzerRapid() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(100);
    digitalWrite(BUZZER_PIN, LOW);  delay(100);
  }
}

// DOUBLE — 2 medium beeps
void buzzerDouble() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(200);
    digitalWrite(BUZZER_PIN, LOW);  delay(300);
  }
}

// SLOW — 1 long beep
void buzzerSlow() {
  digitalWrite(BUZZER_PIN, HIGH); delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}
```

---

## 10. Summary Flow

```
Every 120ms: Read Left, Front, Right distances
                      │
         ┌────────────┼────────────┐
         │            │            │
   Rate Check    Zone Update   Pattern Check
  (FAST/SLOW)  (with Hysteresis)  (Corner/Deadend/Corridor)
         │            │            │
         └────────────┼────────────┘
                      │
               Decide Priority
              (0=Emergency → 4=Info)
                      │
             ┌────────┴────────┐
         Buzzer Pattern     Audio Track
         (Immediate)       (if idle or higher priority)
```
