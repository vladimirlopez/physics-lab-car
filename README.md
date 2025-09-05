# Arduino Physics Lab Car

This repository contains multiple Arduino sketches for controlling and testing self-driving cars for physics lab experiments. The main goal is to achieve straight-line motion with minimal manual calibration, suitable for classroom use with multiple cars.

## Folder Structure
- `src/` — All Arduino sketches (.ino files)
- `wiring_diagram.txt` — Pinout and wiring reference
- `.gitignore` — Ignore build and editor files
- `setup_arduino.sh`, `check_connection.sh` — Setup and diagnostic scripts

## Main Sketches
- `self_correcting_car.ino` — Uses line tracking sensors for automatic straight-line correction (recommended)
- `alternating_correction_car.ino` — Software-only solution, alternates motor corrections to average out drift
- `straight_line_motion.ino` — Simple constant velocity motion, no correction
- Other sketches are for diagnostics, IR remote, and advanced features

## Usage
1. Upload the desired sketch from `src/` to your Arduino car.
2. For best results, use `self_correcting_car.ino` and place black tape on the floor for line following.
3. For quick deployment on many cars, use `alternating_correction_car.ino`.

## Setup
- See `setup_arduino.sh` for automated environment setup on macOS.
- See `wiring_diagram.txt` for hardware connections.

## License
MIT
