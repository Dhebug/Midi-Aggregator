# Midi-Aggregator
Some experimental project started a few years ago to see if I could build a minimalistic DAW designed to work with legacy MIDI devices

This is not supposed to be a fully portable and configurable piece of software, it's pretty much hardcoded to my equipment, which at the time was the following:

Inputs:
- Yamaha Reface YC (used as master keyboard)
- Akai MPK mini (used as control device and drum kit)

Outputs:
- Yamaha TX7 - FM Expander / Tone Generator
- Korg NX5R - Sound module
- Yamaha MU100 - Sound module
- Roland SC88vl - Sound module
- Yamaha FX500 - Effect processor
- Boss SE70 - Effect processor

The rendering is done using the [SFML](https://www.sfml-dev.org) library, and the midi handling uses [RtMidi](http://www.music.mcgill.ca/~gary/rtmidi/).
- [SFML GitHub](https://github.com/SFML/SFML)
- [RtMidi GitHub](https://github.com/thestk/rtmidi)
