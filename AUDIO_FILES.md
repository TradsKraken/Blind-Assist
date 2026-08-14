# Audio Files Guide for DFPlayer Mini

Based on the new, highly detailed 65-state logic table with spatial mapping and rate-of-change detection, you now need exactly **19 MP3 files** loaded onto your DFPlayer Mini's SD card. 

## ⚠️ Important Rules for the SD Card
1. The SD Card must be formatted as **FAT32**.
2. The files must be placed inside a folder named exactly `MP3` (or `mp3`) on the root of the SD card.
3. The files **must** start with a 4-digit number. You can include descriptive text after the numbers (e.g., `0001_Stop.mp3`), but the numbers must come first.

---

## 🎧 Required Audio Files (Short & Direct)

The phrases have been shortened to be as punchy and quick as possible. When walking, long sentences are dangerous because they take too long to hear. 

| File Name (Copy & Paste) | Spoken Phrase / Audio Message | When Used |
|--------------------------|-------------------------------|-----------|
| **`0001_Stop.mp3`** | *"Stop."* | Emergency / Sudden object |
| **`0002_Move_Left.mp3`** | *"Move left."* | Front blocked, left is clear |
| **`0003_Move_Right.mp3`** | *"Move right."* | Front blocked, right is clear |
| **`0004_All_Blocked.mp3`** | *"Stop. All blocked."* | Fully enclosed / trapped |
| **`0005_Narrow_Path.mp3`** | *"Narrow path."* | Squeezed on both sides |
| **`0006_Obstacle_Ahead.mp3`** | *"Obstacle ahead."* | Object approaching in front |
| **`0007_Narrow_Passage.mp3`** | *"Narrow passage."* | Front clear, sides are very tight |
| **`0008_Left_Blocked.mp3`** | *"Left blocked. Move right."* | Obstacle on left only |
| **`0009_Right_Blocked.mp3`** | *"Right blocked. Move left."* | Obstacle on right only |
| **`0010_Path_Clear.mp3`** | *"Path clear."* | All zones open (spoken once) |
| **`0011_Turn_Left.mp3`** | *"Turn left."* | Corner detected on right |
| **`0012_Turn_Right.mp3`** | *"Turn right."* | Corner detected on left |
| **`0013_Dead_End.mp3`** | *"Dead end."* | Walking into a dead end |
| **`0014_Close_Left.mp3`** | *"Close left."* | Object getting close on left |
| **`0015_Close_Right.mp3`** | *"Close right."* | Object getting close on right |
| **`0016_Slow_Down.mp3`** | *"Slow down."* | Approaching an object too fast |
| **`0017_Corridor.mp3`** | *"Corridor. Walk straight."* | Entered a hallway / corridor |
| **`0018_Step_Left.mp3`** | *"Step left."* | Gentle correction away from right |
| **`0019_Step_Right.mp3`** | *"Step right."* | Gentle correction away from left |

---

## 💡 Tips for generating the audio
* **Text-to-Speech (TTS):** Use free online text-to-speech generators (like TTSMaker) to type in these phrases and download them directly as `.mp3` files. Choose a clear, authoritative, and fast-speaking voice.
* **Keep them short:** Do not add pauses or silence to the beginning or end of the audio files. The system needs to react instantly.
