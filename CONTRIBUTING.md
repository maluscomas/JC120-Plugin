# Contributing to JC-120 Plugin

Thanks for your interest in contributing! Here's how to get involved.

## Ways to Contribute

- **Bug reports** — open an Issue with steps to reproduce
- **DSP improvements** — better chorus, reverb, or preamp modelling
- **UI improvements** — closer to the real amp panel
- **Windows build** — help test and release a Windows VST3
- **Cabinet IRs** — 2x12 Alnico impulse responses

## Development Setup

1. Fork the repo and clone it locally
2. Install JUCE 7+ from [juce.com](https://juce.com/get-juce)
3. Open `JC120.jucer` in Projucer
4. Set your JUCE path in Global Paths
5. Export to your IDE and build

## Pull Request Guidelines

- Keep PRs focused — one feature or fix per PR
- Test in at least one DAW before submitting
- Update `CHANGELOG.md` with your changes
- Don't commit build artifacts (`.vst3`, `.component`, `Builds/` folder)

## Reporting Bugs

Please include:
- macOS / Windows version
- DAW name and version
- Steps to reproduce
- Screenshot if relevant
