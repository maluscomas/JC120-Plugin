# JC-120 Jazz Chorus — VST3 / AU Plugin

A faithful software emulation of the legendary **Roland JC-120 Jazz Chorus** amplifier, built with JUCE. Captures the iconic solid-state clean tone and the signature BBD stereo chorus circuit that defined the sound of artists like Andy Summers, Robert Smith, and countless others.

![Plugin UI](docs/screenshot.png)

---

## Features

- **Solid-state preamp** — transistor-accurate soft clipping, no tube colouration
- **3-band tone stack** — Bass, Mid, Treble with Bright switch
- **CE-1 style BBD stereo chorus** — 7ms centre delay, dual LFO paths for true stereo spread (dry left / chorused right)
- **Spring reverb emulation** — Schroeder-style comb + allpass reverb
- **Realistic UI** — Modelled after the JC-120 front panel with silver aluminium finish, black chicken-head knobs, section panels, input jacks and LED indicators
- **Formats:** VST3 + AU (macOS), VST3 (Windows)
- **Stereo in/out**, no MIDI required

---

## Controls

| Knob | Range | Description |
|---|---|---|
| Volume | 0–1 | Master output level |
| Drive | 0–1 | Transistor preamp grit (keep low for classic clean) |
| Bright | 0–1 | High-frequency air boost switch |
| Bass | 0–1 | Low shelf EQ |
| Mid | 0–1 | Midrange boost/cut |
| Treble | 0–1 | High shelf EQ |
| Rate | 0.1–5 Hz | Chorus LFO speed |
| Depth | 0–1 | Chorus modulation depth |
| Mix | 0–1 | Chorus dry/wet blend |
| Reverb | 0–1 | Spring reverb amount |

### Recommended Starting Settings (Classic JC-120 Clean Tone)
```
Volume:  0.8    Drive:  0.05   Bright: 1.0
Bass:    0.5    Mid:    0.5    Treble: 0.6
Rate:    0.7    Depth:  0.65   Mix:    0.5    Reverb: 0.15
```

---

## Installation

### macOS
1. Download the latest release from the [Releases](../../releases) page
2. Copy `JC120.vst3` to `~/Library/Audio/Plug-Ins/VST3/`
3. Copy `JC120.component` to `~/Library/Audio/Plug-Ins/Components/` (for AU)
4. If macOS blocks the plugin, run in Terminal:
   ```bash
   sudo xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/JC120.vst3
   sudo xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/Components/JC120.component
   ```
5. Rescan plugins in your DAW

### Windows
1. Download the latest release
2. Copy `JC120.vst3` to `C:\Program Files\Common Files\VST3\`
3. Rescan plugins in your DAW

---

## Building from Source

### Requirements
- [JUCE 7+](https://juce.com/get-juce)
- **macOS:** Xcode 14+ with Command Line Tools
- **Windows:** Visual Studio 2022
- CMake 3.22+

### Steps
1. Clone the repo:
   ```bash
   git clone https://github.com/Maluscomas/JC120-Plugin.git
   cd JC120-Plugin
   ```
2. Open `JC120.jucer` in **Projucer**
3. Set your JUCE path in **Projucer → Global Paths**
4. Click **Save Project and Open in IDE**
5. Build in Xcode (`Cmd+B`) or Visual Studio

The built plugin will be auto-installed to your system VST3 folder.

---

## Compatibility

| DAW | macOS | Windows |
|---|---|---|
| Logic Pro | ✅ AU | — |
| Reaper | ✅ VST3 | ✅ VST3 |
| Ableton Live | ✅ VST3 | ✅ VST3 |
| FL Studio | ✅ VST3 | ✅ VST3 |
| Bitwig | ✅ VST3 | ✅ VST3 |
| Cubase | ✅ VST3 | ✅ VST3 |

---

## Roadmap

- [ ] Cabinet IR loader (2x12 Alnico impulse responses)
- [ ] Vibrato mode (CE-1 style)
- [ ] Dual channel mode (Ch1 + Ch2 like the real amp)
- [ ] Windows build + release
- [ ] Code signing for macOS (no Gatekeeper bypass needed)

---

## License

MIT License — see [LICENSE](LICENSE) for details.

This project is not affiliated with or endorsed by Roland Corporation. "Roland" and "JC-120" are trademarks of Roland Corporation.

---

## Credits

Built with [JUCE](https://juce.com) — the C++ framework for audio applications.
DSP research based on the Roland JC-120 circuit analysis and the CE-1 chorus schematic.
