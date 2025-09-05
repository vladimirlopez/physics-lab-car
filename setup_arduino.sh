#!/bin/bash

# Arduino Setup Script for macOS
# This script will help set up Arduino development environment and diagnose connection issues

echo "🚀 Arduino Setup Script for macOS"
echo "=================================="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script is designed for macOS only."
    exit 1
fi

print_info "Starting Arduino setup process..."
echo

# Step 1: Check for Homebrew and install if needed
echo "Step 1: Checking Homebrew installation..."
if ! command -v brew &> /dev/null; then
    print_warning "Homebrew not found. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # Add Homebrew to PATH for Apple Silicon Macs
    if [[ $(uname -m) == "arm64" ]]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
else
    print_status "Homebrew is already installed"
fi

# Step 2: Install Arduino CLI
echo
echo "Step 2: Installing Arduino CLI..."
if ! command -v arduino-cli &> /dev/null; then
    brew install arduino-cli
    print_status "Arduino CLI installed successfully"
else
    print_status "Arduino CLI is already installed"
fi

# Step 3: Initialize Arduino CLI
echo
echo "Step 3: Initializing Arduino CLI configuration..."
arduino-cli config init
print_status "Arduino CLI configuration initialized"

# Step 4: Update package index
echo
echo "Step 4: Updating Arduino package index..."
arduino-cli core update-index
print_status "Package index updated"

# Step 5: Install Arduino AVR core (for Uno, Nano, etc.)
echo
echo "Step 5: Installing Arduino AVR core..."
arduino-cli core install arduino:avr
print_status "Arduino AVR core installed"

# Step 6: Install required libraries
echo
echo "Step 6: Installing required libraries..."
arduino-cli lib install "IRremote"
print_status "IRremote library installed"

# Step 7: Check for connected Arduino boards
echo
echo "Step 7: Scanning for connected Arduino boards..."
echo "Please connect your Arduino via USB cable if not already connected."
echo "Press Enter when ready..."
read -r

# List available ports
print_info "Scanning for available serial ports..."
arduino-cli board list

# Store the output for analysis
BOARD_LIST=$(arduino-cli board list)

if echo "$BOARD_LIST" | grep -q "Arduino"; then
    print_status "Arduino board detected!"
    PORT=$(echo "$BOARD_LIST" | grep "Arduino" | awk '{print $1}' | head -1)
    BOARD=$(echo "$BOARD_LIST" | grep "Arduino" | awk '{print $6}' | head -1)
    print_info "Found board: $BOARD on port: $PORT"
else
    print_warning "No Arduino boards automatically detected."
    print_info "Available ports:"
    ls -la /dev/cu.* 2>/dev/null | grep -E "(usbmodem|usbserial)" || echo "No USB serial devices found"
    
    echo
    print_info "Troubleshooting steps:"
    echo "1. Make sure your Arduino is connected via USB"
    echo "2. Try a different USB cable (data cable, not just charging cable)"
    echo "3. Try a different USB port"
    echo "4. Check if the Arduino's power LED is on"
    echo "5. Press the reset button on the Arduino"
    
    echo
    echo "If you see a device like /dev/cu.usbmodem* or /dev/cu.usbserial*, that's likely your Arduino."
    echo "Please enter the port manually (e.g., /dev/cu.usbmodem14101):"
    read -r PORT
    
    echo "Please enter the board type (e.g., arduino:avr:uno for Arduino Uno):"
    echo "Common boards:"
    echo "  - Arduino Uno: arduino:avr:uno"
    echo "  - Arduino Nano: arduino:avr:nano"
    echo "  - Arduino Leonardo: arduino:avr:leonardo"
    read -r BOARD
fi

# Step 8: Test compilation
echo
echo "Step 8: Testing sketch compilation..."
SKETCH_DIR="/Users/vladimir.lopez/Library/CloudStorage/GoogleDrive-vladimir.lopez@kinkaid.org/My Drive/01Physics/Resources/Arduino Car /New Average Velocity"
SKETCH_PATH="$SKETCH_DIR/physics_lab_car.ino"

if arduino-cli compile --fqbn "$BOARD" "$SKETCH_PATH"; then
    print_status "Sketch compiled successfully!"
else
    print_error "Sketch compilation failed. Check the code for errors."
    exit 1
fi

# Step 9: Upload the sketch
if [[ -n "$PORT" && -n "$BOARD" ]]; then
    echo
    echo "Step 9: Uploading sketch to Arduino..."
    echo "Board: $BOARD"
    echo "Port: $PORT"
    echo
    
    if arduino-cli upload -p "$PORT" --fqbn "$BOARD" "$SKETCH_PATH"; then
        print_status "Sketch uploaded successfully! 🎉"
        echo
        print_info "Your Arduino is now programmed with the physics lab car code!"
        print_info "Open Serial Monitor to see the output:"
        echo "  arduino-cli monitor -p $PORT"
    else
        print_error "Upload failed. Please check:"
        echo "1. The correct port is selected"
        echo "2. The correct board type is selected"
        echo "3. The Arduino is properly connected"
        echo "4. No other programs are using the serial port"
    fi
else
    print_warning "Skipping upload - board or port not properly configured"
fi

# Step 10: Create VS Code configuration
echo
echo "Step 10: Creating VS Code Arduino configuration..."

# Create .vscode directory if it doesn't exist
VSCODE_DIR="/Users/vladimir.lopez/Library/CloudStorage/GoogleDrive-vladimir.lopez@kinkaid.org/My Drive/01Physics/Resources/Arduino Car /New Average Velocity/.vscode"
mkdir -p "$VSCODE_DIR"

# Create arduino.json configuration
cat > "$VSCODE_DIR/arduino.json" << EOF
{
    "board": "$BOARD",
    "port": "$PORT",
    "sketch": "physics_lab_car.ino",
    "programmer": "arduino:avrisp"
}
EOF

# Create c_cpp_properties.json for better IntelliSense
cat > "$VSCODE_DIR/c_cpp_properties.json" << EOF
{
    "configurations": [
        {
            "name": "Arduino",
            "includePath": [
                "\${workspaceFolder}/**",
                "~/Library/Arduino15/packages/arduino/hardware/avr/1.8.6/cores/arduino",
                "~/Library/Arduino15/packages/arduino/hardware/avr/1.8.6/libraries/**",
                "~/Documents/Arduino/libraries/**"
            ],
            "defines": [
                "ARDUINO=10819",
                "ARDUINO_AVR_UNO",
                "ARDUINO_ARCH_AVR"
            ],
            "compilerPath": "~/Library/Arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc",
            "cStandard": "c11",
            "cppStandard": "c++11",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
EOF

print_status "VS Code configuration files created"

# Final instructions
echo
echo "🎉 Setup Complete!"
echo "=================="
print_status "Arduino CLI installed and configured"
print_status "Required libraries installed"
print_status "VS Code configuration created"

if [[ -n "$PORT" && -n "$BOARD" ]]; then
    print_status "Sketch uploaded to Arduino"
fi

echo
print_info "Next steps:"
echo "1. Open the physics_lab_car.ino file in VS Code"
echo "2. Install the Arduino extension in VS Code if not already installed"
echo "3. Use Cmd+Shift+P and type 'Arduino: Upload' to upload code"
echo "4. Use Cmd+Shift+P and type 'Arduino: Open Serial Monitor' to view output"
echo
print_info "To test your IR remote, open the Serial Monitor and press buttons."
print_info "The hex codes will be displayed - update them in the Arduino code."
echo
echo "Serial Monitor command: arduino-cli monitor -p $PORT"
echo

print_info "If you need to change board settings, edit:"
echo "$VSCODE_DIR/arduino.json"
