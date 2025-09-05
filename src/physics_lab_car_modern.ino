/*
  Physics Lab Arduino Car - Modern IRremote Library Version
  
  Compatible with IRremote library 4.x
  Uses correct pin assignments and modern API
*/

#include <IRremote.hpp>  // Note: .hpp for version 4.x

// Remote control codes (these may need to be updated based on your remote)
#define N1 16738455    // number 1
#define N2 16750695    // number 2

// Pin definitions
#define RECV_PIN 12    // IR receiver pin

// Motor pin definitions
#define ENB 5          // Left wheel speed
#define IN1 7          // Left wheel forward  
#define IN2 8          // Left wheel reverse
#define IN3 9          // Right wheel reverse
#define IN4 11         // Right wheel forward
#define ENA 6          // Right wheel speed

// Speed definitions
#define SLOW_SPEED 120   
#define FAST_SPEED 250   

// Distance timing (in milliseconds)
#define TIME_60CM_SLOW 4000   
#define TIME_40CM_FAST 1600   
#define TIME_70CM_SLOW 4667   
#define TIME_70CM_FAST 2800   

void forward(int selectedSpeed) {
  analogWrite(ENA, selectedSpeed);
  analogWrite(ENB, selectedSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Moving forward");
}

void back(int selectedSpeed) {
  analogWrite(ENA, selectedSpeed);
  analogWrite(ENB, selectedSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Moving backward");
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println("STOP!");
}

void readySignal() {
  Serial.println("=== PHYSICS LAB CAR READY ===");
  Serial.println("Expected IR codes:");
  Serial.print("Button 1: "); Serial.println(N1);
  Serial.print("Button 2: "); Serial.println(N2);
  Serial.println("==============================");
  
  // 3 fast blinks
  Serial.println("Ready signal: 3 fast blinks...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
  // Ready signal - LED on for 2 seconds
  Serial.println("Waiting for remote input (LED on for 2 seconds)...");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("NOW PRESS '1' OR '2' ON YOUR REMOTE!");
}

void Motion1() {
  Serial.println("=== MOTION 1 SELECTED ===");
  
  // Confirmation blinks
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
  }
  
  // Countdown
  for (int i = 3; i > 0; i--) {
    Serial.print("Starting in ");
    Serial.print(i);
    Serial.println("...");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
  Serial.println("GO! - Motion 1: 60cm forward (slow), 40cm back (fast)");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Phase 1: Forward 60cm at slow speed
  Serial.println("Phase 1: Moving forward 60cm at slow speed");
  forward(SLOW_SPEED);
  delay(TIME_60CM_SLOW);
  stop();
  delay(500);
  
  // Phase 2: Backward 40cm at fast speed
  Serial.println("Phase 2: Moving backward 40cm at fast speed");
  back(FAST_SPEED);
  delay(TIME_40CM_FAST);
  stop();
  
  Serial.println("MOTION 1 COMPLETE!");
  
  // Completion signal - 5 quick blinks
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void Motion2() {
  Serial.println("=== MOTION 2 SELECTED ===");
  
  // Confirmation blinks
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);
    delay(150);
  }
  
  // Countdown
  for (int i = 3; i > 0; i--) {
    Serial.print("Starting in ");
    Serial.print(i);
    Serial.println("...");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
  Serial.println("GO! - Motion 2: 70cm forward (fast), 70cm back (slow)");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Phase 1: Forward 70cm at fast speed
  Serial.println("Phase 1: Moving forward 70cm at fast speed");
  forward(FAST_SPEED);
  delay(TIME_70CM_FAST);
  stop();
  delay(500);
  
  // Phase 2: Backward 70cm at slow speed
  Serial.println("Phase 2: Moving backward 70cm at slow speed");
  back(SLOW_SPEED);
  delay(TIME_70CM_SLOW);
  stop();
  
  Serial.println("MOTION 2 COMPLETE!");
  
  // Completion signal - 5 quick blinks
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000); // Give serial monitor time to connect
  
  // Initialize motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize IR receiver (modern syntax)
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  
  // Make sure motors are stopped
  stop();
  
  // Ready signal
  readySignal();
  
  // Wait for IR command (30 seconds timeout)
  unsigned long startTime = millis();
  bool commandReceived = false;
  
  Serial.println("Listening for IR commands...");
  
  while (!commandReceived && (millis() - startTime < 30000)) {
    // Modern IRremote library syntax
    if (IrReceiver.decode()) {
      uint32_t receivedCode = IrReceiver.decodedIRData.decodedRawData;
      
      Serial.println("================================");
      Serial.print("IR CODE RECEIVED: ");
      Serial.println(receivedCode);
      Serial.print("Protocol: ");
      Serial.println(getProtocolString(IrReceiver.decodedIRData.protocol));
      Serial.println("================================");
      
      // Give immediate visual feedback
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      
      // Check for button matches
      if (receivedCode == N1 || IrReceiver.decodedIRData.command == 0x45) { // Button 1
        Motion1();
        commandReceived = true;
      } else if (receivedCode == N2 || IrReceiver.decodedIRData.command == 0x46) { // Button 2  
        Motion2();
        commandReceived = true;
      } else {
        Serial.println("Button pressed but not recognized as 1 or 2");
        Serial.print("If this should be button 1, use code: ");
        Serial.println(receivedCode);
        Serial.print("Command code: 0x");
        Serial.println(IrReceiver.decodedIRData.command, HEX);
      }
      
      IrReceiver.resume(); // Enable receiving of the next value
      delay(1000); // Debounce delay
    }
    delay(100);
  }
  
  if (!commandReceived) {
    Serial.println("TIMEOUT - No valid command received");
    // Rapid blink for timeout
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }
}

void loop() {
  // Continue listening for more commands
  if (IrReceiver.decode()) {
    uint32_t receivedCode = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("Additional IR code: ");
    Serial.println(receivedCode);
    
    // Quick feedback blink
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    
    IrReceiver.resume();
    delay(500);
  }
  delay(100);
}
