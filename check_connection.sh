#!/bin/bash

# Quick Arduino Diagnostic Script
# Use this to check if your Arduino is connected properly

echo "🔍 Arduino Connection Diagnostic"
echo "================================"
echo

# Check for USB devices
echo "Step 1: Checking for USB serial devices..."
echo "Looking for devices in /dev/cu.*:"

USB_DEVICES=$(ls -la /dev/cu.* 2>/dev/null | grep -E "(usbmodem|usbserial|SLAB|wchusbserial)")

if [[ -n "$USB_DEVICES" ]]; then
    echo "✅ Found potential Arduino devices:"
    echo "$USB_DEVICES"
else
    echo "❌ No USB serial devices found."
    echo
    echo "Troubleshooting:"
    echo "1. Connect your Arduino via USB"
    echo "2. Try a different USB cable (must be a data cable, not just power)"
    echo "3. Try a different USB port"
    echo "4. Check if the Arduino's power LED lights up"
    echo "5. Press the reset button on the Arduino"
fi

echo
echo "Step 2: Checking system information..."
echo "USB devices from system_profiler:"
system_profiler SPUSBDataType | grep -A 10 -B 5 -i "arduino\|ch340\|cp210\|ftdi"

echo
echo "Step 3: Testing different USB cable orientations..."
echo "If using USB-C, try flipping the cable connector"

echo
echo "Step 4: Arduino board identification help..."
echo "Common Arduino boards and their identifiers:"
echo "- Arduino Uno R3: Usually shows as 'Arduino Uno' or vendor ID 2341"
echo "- Arduino Nano: May show as 'CH340' or 'CP2102'"
echo "- Generic clones: Often show as 'CH340G' or 'CP2102'"

echo
echo "If you see any devices above, your Arduino is likely connected."
echo "Run the full setup script with: ./setup_arduino.sh"
