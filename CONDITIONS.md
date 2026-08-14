# Conditions & Logic Reference

Exhaustive list of every sensor-state combination the firmware can see, and
the exact message it produces for each. This is the full truth table behind
the `decideMessage()` function in `blind_assist.ino` — `README.md` shows the
condensed version, this file spells out all 27 cases.

## 1. Zone thresholds (per sensor)

| Zone    | Enter when distance is... | Exit back when distance is... |
|---------|----------------------------|--------------------------------|
| Danger  | ≤ 30 cm                    | > 35 cm                        |
| Warning | 30–80 cm                   | > 90 cm                        |
| Clear   | > 80 cm                    | ≤ 30 cm (drops straight to Danger) |

The enter/exit gap (30 vs 35, 80 vs 90) is hysteresis: it stops a reading
that's sitting right on a boundary from flipping the zone — and re-triggering
speech — every single sensor loop.

## 2. Full condition table (all 27 Front × Left × Right combinations)

| # | Front | Left | Right | Track | Message | Priority |
|---|-------|------|-------|-------|---------|----------|
| 1 | Danger | Clear | Clear | 1 | Stop. Obstacle ahead. | 1 |
| 2 | Danger | Clear | Warning | 1 | Stop. Obstacle ahead. | 1 |
| 3 | Danger | Clear | Danger | 2 | Obstacle ahead. Move left. | 1 |
| 4 | Danger | Warning | Clear | 1 | Stop. Obstacle ahead. | 1 |
| 5 | Danger | Warning | Warning | 1 | Stop. Obstacle ahead. | 1 |
| 6 | Danger | Warning | Danger | 2 | Obstacle ahead. Move left. | 1 |
| 7 | Danger | Danger | Clear | 3 | Obstacle ahead. Move right. | 1 |
| 8 | Danger | Danger | Warning | 3 | Obstacle ahead. Move right. | 1 |
| 9 | Danger | Danger | Danger | 4 | Stop. Obstacles all around. Please wait. | 1 |
| 10 | Warning | Clear | Clear | 6 | Caution. Obstacle ahead. | 3 |
| 11 | Warning | Clear | Warning | 6 | Caution. Obstacle ahead. | 3 |
| 12 | Warning | Clear | Danger | 6 | Caution. Obstacle ahead. | 3 |
| 13 | Warning | Warning | Clear | 6 | Caution. Obstacle ahead. | 3 |
| 14 | Warning | Warning | Warning | 6 | Caution. Obstacle ahead. | 3 |
| 15 | Warning | Warning | Danger | 6 | Caution. Obstacle ahead. | 3 |
| 16 | Warning | Danger | Clear | 6 | Caution. Obstacle ahead. | 3 |
| 17 | Warning | Danger | Warning | 6 | Caution. Obstacle ahead. | 3 |
| 18 | Warning | Danger | Danger | 5 | Caution. Narrow path ahead. | 2 |
| 19 | Clear | Clear | Clear | 10 | Path clear. | 4 |
| 20 | Clear | Clear | Warning | 10 | Path clear. | 4 |
| 21 | Clear | Clear | Danger | 9 | Obstacle on your right. Move left. | 2 |
| 22 | Clear | Warning | Clear | 10 | Path clear. | 4 |
| 23 | Clear | Warning | Warning | 10 | Path clear. | 4 |
| 24 | Clear | Warning | Danger | 9 | Obstacle on your right. Move left. | 2 |
| 25 | Clear | Danger | Clear | 8 | Obstacle on your left. Move right. | 2 |
| 26 | Clear | Danger | Warning | 8 | Obstacle on your left. Move right. | 2 |
| 27 | Clear | Danger | Danger | 7 | Caution. Narrow passage. | 2 |

### Reading the table

- **Front is the tie-breaker of last resort**: any Front = Danger forces a
  "stop" family message (rows 1–9) regardless of what the sides are doing,
  because the user is about to walk straight into something.
- **Front = Warning** only ever produces "Caution, obstacle ahead" unless
  *both* sides are already Danger too (row 18), in which case it upgrades to
  "narrow path ahead" — there's nowhere left to step.
- **Front = Clear** is where left/right steering messages happen (rows 21,
  24, 25, 26): only one side is blocked, so the message tells the user which
  way is open.
- **Both sides Danger with Front Clear** (row 27) is treated as a squeeze
  ("narrow passage"), not a directional instruction, since neither side is
  safer than the other.
- **Row 19/20/22/23** are the only fully-relaxed states — "Path clear" is
  spoken once on the transition into one of these rows, not repeated.

## 3. Priority levels

| Priority | Meaning | Tracks |
|----------|---------|--------|
| 1 | Immediate danger — can interrupt anything currently playing | 1, 2, 3, 4 |
| 2 | Side obstacle / squeeze — can interrupt priority 3–4 | 5, 7, 8, 9 |
| 3 | General front caution | 6 |
| 4 | Informational only, never repeats | 10 |

## 4. Timing / anti-spam conditions

| Condition | Behavior |
|-----------|----------|
| Message track changes and player is idle | Speak immediately |
| Message track changes, player busy, new priority is more urgent (lower number) | Interrupt current clip and speak the new one |
| Message track changes, player busy, new priority is equal/less urgent | Wait for current clip to finish |
| Same message track stays active | Re-speak every 2.5 s if priority 1, every 5 s if priority 2–3 |
| Message is "Path clear" (track 10) | Speak once on the transition into it, never repeat on a timer |
| Sensor read | Runs every 120 ms (HC-SR04 needs settle time between pings) |
