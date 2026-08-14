#include <Arduino.h>
#include <NewPing.h>
#include <DFRobotDFPlayerMini.h>

/*
 * Blind Assist Navigation Belt — Full Implementation
 * Implements all scenarios from LOGIC_TABLE.md:
 *   - 4 distance zones: CLEAR, WARNING, CLOSE, DANGER (with hysteresis)
 *   - Rate-of-change: FAST APPROACH detection per sensor
 *   - 5 buzzer patterns: EMERGENCY, SOS, RAPID, DOUBLE, SLOW, SILENT
 *   - Spatial patterns: Dead End, Corridor Entry
 *   - 19 audio tracks via DFPlayer Mini
 *   - Priority interrupt system (P0=Emergency → P4=Informational)
 *
 * Track Map (SD card /MP3/ folder):
 *   01 "Stop."                      07 "Narrow passage."
 *   02 "Move left."                 08 "Left blocked. Move right."
 *   03 "Move right."                09 "Right blocked. Move left."
 *   04 "Stop. All blocked."         10 "Path clear."
 *   05 "Narrow path."               11 "Turn left."
 *   06 "Obstacle ahead."            12 "Turn right."
 *                                   13 "Dead end."
 *                                   14 "Close left."
 *                                   15 "Close right."
 *                                   16 "Slow down."
 *                                   17 "Corridor. Walk straight."
 *                                   18 "Step left."
 *                                   19 "Step right."
 */

// ====================================================================
// PIN DEFINITIONS
// ====================================================================
#define TRIG_FRONT        26
#define ECHO_FRONT        25
#define TRIG_LEFT         13
#define ECHO_LEFT         12
#define TRIG_RIGHT        33
#define ECHO_RIGHT        32
#define DFPLAYER_BUSY_PIN  4   // LOW while a track is playing
#define VIB_MOTOR_PIN     27
#define BUZZER_PIN         5
#define TACTILE_SW_PIN    15
#define RX2_PIN           16
#define TX2_PIN           17

// ====================================================================
// ZONE THRESHOLDS (all with hysteresis — enter < exit)
// ====================================================================
#define MAX_DISTANCE       400.0f
#define DANGER_ENTER_CM     30.0f
#define DANGER_EXIT_CM      35.0f
#define CLOSE_ENTER_CM      60.0f
#define CLOSE_EXIT_CM       70.0f
#define WARNING_ENTER_CM   120.0f
#define WARNING_EXIT_CM    130.0f
#define FAST_APPROACH_CM    15.0f  // cm drop per cycle = FAST

// ====================================================================
// TIMING
// ====================================================================
#define PING_INTERVAL_MS   120    // sensor poll rate
#define REPEAT_P0_MS      1500   // emergency repeat
#define REPEAT_P1_MS      2500   // danger repeat
#define REPEAT_P2_MS      4000   // side-danger repeat
#define REPEAT_P3_MS      6000   // caution repeat

// ====================================================================
// ENUMS & STRUCTS
// ====================================================================
enum Zone    { ZONE_CLEAR, ZONE_WARNING, ZONE_CLOSE, ZONE_DANGER };
enum BuzzPat { BUZZ_SILENT, BUZZ_SLOW, BUZZ_DOUBLE, BUZZ_RAPID, BUZZ_SOS, BUZZ_EMERGENCY };
struct Decision { int track; int priority; BuzzPat buzz; };

// ====================================================================
// FORWARD DECLARATIONS
// ====================================================================
Zone     updateZone(Zone current, float dist);
bool     isFastApproach(float prev, float curr);
bool     isDeadEnd();
bool     isCorridorEntry();
Decision decideMessage(Zone front, Zone left, Zone right,
                       float dFront, float dLeft, float dRight,
                       bool fastFront, bool fastLeft, bool fastRight);
void     runBuzzer(BuzzPat pattern);
void     vibPulse();
void     playTrack(int track, int priority);

// ====================================================================
// OBJECTS
// ====================================================================
NewPing sonarFront(TRIG_FRONT, ECHO_FRONT, (int)MAX_DISTANCE);
NewPing sonarLeft (TRIG_LEFT,  ECHO_LEFT,  (int)MAX_DISTANCE);
NewPing sonarRight(TRIG_RIGHT, ECHO_RIGHT, (int)MAX_DISTANCE);

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini player;

// ====================================================================
// STATE VARIABLES
// ====================================================================
Zone  zoneFront = ZONE_CLEAR, zoneLeft = ZONE_CLEAR, zoneRight = ZONE_CLEAR;
float prevFront = MAX_DISTANCE, prevLeft = MAX_DISTANCE, prevRight = MAX_DISTANCE;

// Ring buffer — last 3 readings for pattern detection
Zone  histFront[3] = {ZONE_CLEAR, ZONE_CLEAR, ZONE_CLEAR};
Zone  histLeft [3] = {ZONE_CLEAR, ZONE_CLEAR, ZONE_CLEAR};
Zone  histRight[3] = {ZONE_CLEAR, ZONE_CLEAR, ZONE_CLEAR};
uint8_t histIdx = 0;

int           activeTrack    = 0;
int           activePriority = 99;
unsigned long lastPingTime   = 0;
unsigned long lastSpeakTime  = 0;

// ====================================================================
// ZONE UPDATE — hysteresis prevents flickering on boundaries
// ====================================================================
Zone updateZone(Zone current, float dist) {
  switch (current) {
    case ZONE_DANGER:
      if (dist > WARNING_EXIT_CM)                            return ZONE_CLEAR;
      if (dist > CLOSE_EXIT_CM)                              return ZONE_WARNING;
      if (dist > DANGER_EXIT_CM)                             return ZONE_CLOSE;
      return ZONE_DANGER;

    case ZONE_CLOSE:
      if (dist <= DANGER_ENTER_CM)                           return ZONE_DANGER;
      if (dist > WARNING_EXIT_CM)                            return ZONE_CLEAR;
      if (dist > CLOSE_EXIT_CM)                              return ZONE_WARNING;
      return ZONE_CLOSE;

    case ZONE_WARNING:
      if (dist <= DANGER_ENTER_CM)                           return ZONE_DANGER;
      if (dist <= CLOSE_ENTER_CM)                            return ZONE_CLOSE;
      if (dist > WARNING_EXIT_CM)                            return ZONE_CLEAR;
      return ZONE_WARNING;

    case ZONE_CLEAR:
    default:
      if (dist <= DANGER_ENTER_CM)                           return ZONE_DANGER;
      if (dist <= CLOSE_ENTER_CM)                            return ZONE_CLOSE;
      if (dist <= WARNING_ENTER_CM)                          return ZONE_WARNING;
      return ZONE_CLEAR;
  }
}

// ====================================================================
// RATE OF CHANGE — detects fast-approaching objects
// ====================================================================
bool isFastApproach(float prev, float curr) {
  return (prev - curr) > FAST_APPROACH_CM;
}

// ====================================================================
// PATTERN DETECTION — dead end
// All 3 sensors converging toward DANGER over last 3 readings
// ====================================================================
bool isDeadEnd() {
  uint8_t a = histIdx;             // oldest
  uint8_t b = (histIdx + 1) % 3;
  uint8_t c = (histIdx + 2) % 3;  // newest (just written)
  return (histFront[a] <= ZONE_WARNING && histFront[b] >= ZONE_CLOSE && histFront[c] == ZONE_DANGER &&
          histLeft [c] >= ZONE_CLOSE &&
          histRight[c] >= ZONE_CLOSE);
}

// ====================================================================
// PATTERN DETECTION — corridor entry
// Both sides jump from CLEAR to WARNING while front stays clear
// ====================================================================
bool isCorridorEntry() {
  uint8_t prev = (histIdx + 1) % 3; // reading from 2 cycles ago
  uint8_t curr = (histIdx + 2) % 3; // newest
  return (histLeft [prev] == ZONE_CLEAR && histLeft [curr] >= ZONE_WARNING &&
          histRight[prev] == ZONE_CLEAR && histRight[curr] >= ZONE_WARNING &&
          zoneFront == ZONE_CLEAR);
}

// ====================================================================
// BUZZER PATTERNS
// ====================================================================
void runBuzzer(BuzzPat pattern) {
  switch (pattern) {
    case BUZZ_EMERGENCY:
      for (int i = 0; i < 5; i++) {
        digitalWrite(BUZZER_PIN, HIGH); delay(30);
        digitalWrite(BUZZER_PIN, LOW);  delay(30);
      }
      break;
    case BUZZ_SOS:
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH); delay(50);
        digitalWrite(BUZZER_PIN, LOW);  delay(50);
      }
      break;
    case BUZZ_RAPID:
      for (int i = 0; i < 2; i++) {
        digitalWrite(BUZZER_PIN, HIGH); delay(100);
        digitalWrite(BUZZER_PIN, LOW);  delay(100);
      }
      break;
    case BUZZ_DOUBLE:
      for (int i = 0; i < 2; i++) {
        digitalWrite(BUZZER_PIN, HIGH); delay(200);
        digitalWrite(BUZZER_PIN, LOW);  delay(300);
      }
      break;
    case BUZZ_SLOW:
      digitalWrite(BUZZER_PIN, HIGH); delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      break;
    case BUZZ_SILENT:
    default:
      break;
  }
}

// ====================================================================
// VIBRATION — only for P0 and P1 (immediate physical danger)
// ====================================================================
void vibPulse() {
  digitalWrite(VIB_MOTOR_PIN, HIGH);
  delay(150);
  digitalWrite(VIB_MOTOR_PIN, LOW);
}

// ====================================================================
// AUDIO PLAYBACK — respects priority, cooldown, and BUSY pin
// ====================================================================
void playTrack(int track, int priority) {
  unsigned long now = millis();
  bool playerBusy = (digitalRead(DFPLAYER_BUSY_PIN) == LOW);

  // Determine repeat interval for this priority
  unsigned long interval;
  if      (priority == 0) interval = REPEAT_P0_MS;
  else if (priority == 1) interval = REPEAT_P1_MS;
  else if (priority == 2) interval = REPEAT_P2_MS;
  else if (priority == 3) interval = REPEAT_P3_MS;
  else                    interval = 0; // P4 = speak once, never repeat

  // Same track still active — check cooldown
  if (track == activeTrack) {
    if (priority == 4) return;                        // informational, never repeat
    if (now - lastSpeakTime < interval) return;       // still in cooldown
  }

  // Different track — interrupt only if higher priority
  if (playerBusy) {
    if (priority >= activePriority) return;           // not urgent enough to cut in
    player.stop();
    delay(50);
  }

  Serial.printf("  >> Track %02d (P%d)\n", track, priority);
  player.play(track);
  activeTrack    = track;
  activePriority = priority;
  lastSpeakTime  = now;
}

// ====================================================================
// CORE DECISION ENGINE — maps all 65+ conditions to a Decision
// ====================================================================
Decision decideMessage(Zone front, Zone left, Zone right,
                       float dFront, float dLeft, float dRight,
                       bool fastFront, bool fastLeft, bool fastRight) {

  // ---- P0: Emergency overrides ----
  if (isDeadEnd())
    return {13, 0, BUZZ_EMERGENCY};                               // "Dead end."

  if (fastFront && front >= ZONE_CLOSE)
    return {16, 0, BUZZ_EMERGENCY};                               // "Slow down."

  if ((fastLeft && left == ZONE_DANGER) || (fastRight && right == ZONE_DANGER))
    return {1, 0, BUZZ_EMERGENCY};                                // "Stop." — sudden side

  // ---- Corridor entry pattern ----
  if (isCorridorEntry())
    return {17, 3, BUZZ_SILENT};                                  // "Corridor. Walk straight."

  // ---- P1: Front DANGER ----
  if (front == ZONE_DANGER) {
    if (left == ZONE_DANGER && right == ZONE_DANGER)
      return {4,  1, BUZZ_EMERGENCY};                             // "Stop. All blocked."
    if (left == ZONE_DANGER)
      return {3,  1, BUZZ_SOS};                                   // "Move right."
    if (right == ZONE_DANGER)
      return {2,  1, BUZZ_SOS};                                   // "Move left."
    // Both sides safe — steer toward whichever has more space
    return (dLeft >= dRight) ? Decision{2, 1, BUZZ_SOS}
                             : Decision{3, 1, BUZZ_SOS};
  }

  // ---- P1: Front CLOSE ----
  if (front == ZONE_CLOSE) {
    if (left == ZONE_DANGER && right == ZONE_DANGER)
      return {4,  1, BUZZ_EMERGENCY};                             // "Stop. All blocked."
    if (left == ZONE_DANGER)
      return {3,  1, BUZZ_SOS};                                   // "Move right."
    if (right == ZONE_DANGER)
      return {2,  1, BUZZ_SOS};                                   // "Move left."
    if (left == ZONE_CLOSE && right == ZONE_CLOSE)
      return {5,  1, BUZZ_SOS};                                   // "Narrow path."
    // Steer toward clearer side
    return (dLeft >= dRight) ? Decision{2, 1, BUZZ_RAPID}
                             : Decision{3, 1, BUZZ_RAPID};
  }

  // ---- P2–P3: Front WARNING ----
  if (front == ZONE_WARNING) {
    if (left == ZONE_DANGER && right == ZONE_DANGER)
      return {5,  1, BUZZ_SOS};                                   // "Narrow path."
    if (left == ZONE_DANGER)
      return {3,  2, BUZZ_RAPID};                                 // "Move right."
    if (right == ZONE_DANGER)
      return {2,  2, BUZZ_RAPID};                                 // "Move left."
    if (left == ZONE_CLOSE && right == ZONE_CLOSE)
      return {17, 3, BUZZ_DOUBLE};                                // "Corridor. Walk straight."
    if (left == ZONE_CLOSE)
      return {19, 3, BUZZ_DOUBLE};                                // "Step right."
    if (right == ZONE_CLOSE)
      return {18, 3, BUZZ_DOUBLE};                                // "Step left."
    return {6,  3, BUZZ_SLOW};                                    // "Obstacle ahead."
  }

  // ---- Front CLEAR — react to sides only ----
  if (left == ZONE_DANGER && right == ZONE_DANGER)
    return {7,  2, BUZZ_SLOW};                                    // "Narrow passage."
  if (left == ZONE_DANGER)
    return {8,  2, BUZZ_RAPID};                                   // "Left blocked. Move right."
  if (right == ZONE_DANGER)
    return {9,  2, BUZZ_RAPID};                                   // "Right blocked. Move left."
  if (left == ZONE_CLOSE && right == ZONE_CLOSE)
    return {17, 3, BUZZ_SLOW};                                    // "Corridor. Walk straight."
  if (left == ZONE_CLOSE)
    return {14, 4, BUZZ_SILENT};                                  // "Close left." (gentle awareness)
  if (right == ZONE_CLOSE)
    return {15, 4, BUZZ_SILENT};                                  // "Close right."

  // Fully open
  return {10, 4, BUZZ_SILENT};                                    // "Path clear." (once)
}

// ====================================================================
// SETUP
// ====================================================================
void setup() {
  Serial.begin(115200);

  pinMode(VIB_MOTOR_PIN,     OUTPUT);
  pinMode(BUZZER_PIN,        OUTPUT);
  pinMode(DFPLAYER_BUSY_PIN, INPUT_PULLUP);
  pinMode(TACTILE_SW_PIN,    INPUT_PULLUP);
  digitalWrite(VIB_MOTOR_PIN, LOW);
  digitalWrite(BUZZER_PIN,    LOW);

  dfSerial.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  Serial.println(F("Initializing DFPlayer..."));

  if (!player.begin(dfSerial)) {
    Serial.println(F("DFPlayer init failed! Check wiring and SD card."));
    // Continue anyway — sensors still work for debug
  } else {
    Serial.println(F("DFPlayer Mini online."));
    player.volume(30); // Max volume
  }

  Serial.println(F("Blind Assist Ready. Full logic active."));
  Serial.println(F("Format: L:[dist]cm[Zone] F:[dist]cm[Zone] R:[dist]cm[Zone]"));
}

// ====================================================================
// LOOP
// ====================================================================
void loop() {
  unsigned long now = millis();

  // ---- Sensor poll ----
  if (now - lastPingTime >= PING_INTERVAL_MS) {
    lastPingTime = now;

    float dFront = sonarFront.ping_cm();
    float dLeft  = sonarLeft.ping_cm();
    float dRight = sonarRight.ping_cm();

    // ping_cm() returns 0 when out of range — treat as MAX
    if (dFront == 0) dFront = MAX_DISTANCE;
    if (dLeft  == 0) dLeft  = MAX_DISTANCE;
    if (dRight == 0) dRight = MAX_DISTANCE;

    // Rate-of-change check
    bool fastFront = isFastApproach(prevFront, dFront);
    bool fastLeft  = isFastApproach(prevLeft,  dLeft);
    bool fastRight = isFastApproach(prevRight, dRight);
    prevFront = dFront;
    prevLeft  = dLeft;
    prevRight = dRight;

    // Update zones with hysteresis
    zoneFront = updateZone(zoneFront, dFront);
    zoneLeft  = updateZone(zoneLeft,  dLeft);
    zoneRight = updateZone(zoneRight, dRight);

    // Store in history ring buffer (oldest index = histIdx)
    histFront[histIdx] = zoneFront;
    histLeft [histIdx] = zoneLeft;
    histRight[histIdx] = zoneRight;
    histIdx = (histIdx + 1) % 3;

    // Serial debug output
    const char* zStr[] = {"Clr", "Wrn", "Cls", "DNG"};
    Serial.printf("L:%5.1fcm[%s] F:%5.1fcm[%s] R:%5.1fcm[%s]%s%s%s\n",
      dLeft,  zStr[zoneLeft],
      dFront, zStr[zoneFront],
      dRight, zStr[zoneRight],
      fastFront ? " FAST-F" : "",
      fastLeft  ? " FAST-L" : "",
      fastRight ? " FAST-R" : "");

    // ---- Decide & act ----
    Decision d = decideMessage(zoneFront, zoneLeft, zoneRight,
                               dFront, dLeft, dRight,
                               fastFront, fastLeft, fastRight);

    // Vibration motor for P0 and P1 only
    if (d.priority <= 1) vibPulse();

    // Buzzer
    runBuzzer(d.buzz);

    // Audio
    playTrack(d.track, d.priority);
  }

  // ---- Tactile switch: replay last spoken track ----
  if (digitalRead(TACTILE_SW_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(TACTILE_SW_PIN) == LOW) {
      Serial.println("Switch: replaying last track");
      if (activeTrack > 0) {
        player.play(activeTrack);
        lastSpeakTime = millis();
      }
      while (digitalRead(TACTILE_SW_PIN) == LOW); // wait for release
    }
  }
}
