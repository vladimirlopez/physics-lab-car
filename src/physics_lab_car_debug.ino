/*
  DEBUG VERSION - Physics Lab Arduino Car
  
  This version will help us see exactly what's happening:
  - Shows all IR codes received
  - Extended timing for button presses
  - More verbose feedback
*/

#include <IRremote.h>

// Remote control codes (from your working sketch)
#define N1 16738455    // number 1
#define N2 16750695    // number 2

// Pin definitions (from your working sketch)
#define RECV_PIN 12    // IR receiver pin

// Motor pin definitions (from your working sketch)
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

IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long val;

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
  Serial.println("=== DEBUG MODE - PHYSICS LAB CAR ===");
  Serial.println("Expected codes:");
  Serial.print("Button 1: "); Serial.println(N1);
  Serial.print("Button 2: "); Serial.println(N2);
  Serial.println("=====================================");
  
  // 3 fast blinks
  Serial.println("Showing 3 fast blinks...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
  // Longer ready signal
  Serial.println("Ready signal - LED on for 3 seconds...");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("NOW PRESS YOUR REMOTE BUTTON!");
}

void Motion1() {
  Serial.println("=== MOTION 1 STARTING ===");
  
  // Confirmation blinks
  for (int i = 0; i < 5; i++) {
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
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
  Serial.println("GO! - Motion 1 executing");
  
  // Phase 1: Forward
  Serial.println("Phase 1: Moving forward 60cm at slow speed");
  forward(SLOW_SPEED);
  delay(TIME_60CM_SLOW);
  stop();
  delay(1000);
  
  // Phase 2: Backward  
  Serial.println("Phase 2: Moving backward 40cm at fast speed");
  back(FAST_SPEED);
  delay(TIME_40CM_FAST);
  stop();
  
  Serial.println("MOTION 1 COMPLETE!");
  
  // Completion signal
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void Motion2() {
  Serial.println("=== MOTION 2 STARTING ===");
  
  // Confirmation blinks
  for (int i = 0; i < 5; i++) {
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
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
  Serial.println("GO! - Motion 2 executing");
  
  // Phase 1: Forward
  Serial.println("Phase 1: Moving forward 70cm at fast speed");
  forward(FAST_SPEED);
  delay(TIME_70CM_FAST);
  stop();
  delay(1000);
  
  // Phase 2: Backward
  Serial.println("Phase 2: Moving backward 70cm at slow speed");
  back(SLOW_SPEED);
  delay(TIME_70CM_SLOW);
  stop();
  
  Serial.println("MOTION 2 COMPLETE!");
  
  // Completion signal
  for (int i = 0; i < 10; i++) {
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
  
  // Initialize IR receiver
  irrecv.enableIRIn();
  
  // Make sure motors are stopped
  stop();
  
  // Ready signal
  readySignal();
  
  // Extended wait for IR command (60 seconds timeout)
  unsigned long startTime = millis();
  bool commandReceived = false;
  
  Serial.println("Listening for IR commands... (60 second timeout)");
  Serial.println("Press any button to see its code!");
  
  while (!commandReceived && (millis() - startTime < 60000)) {
    if (irrecv.decode(&results)) {
      val = results.value;
      
      Serial.println("================================");
      Serial.print("IR CODE RECEIVED: ");
      Serial.println(val);
      Serial.print("In HEX: 0x");
      Serial.println(val, HEX);
      Serial.println("================================");
      
      // Give immediate feedback for ANY button
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      
      irrecv.resume();
      
      // Check if it matches our expected codes
      if (val == N1) {
        Serial.println(">>> BUTTON 1 MATCHED! Starting Motion 1 <<<");
        Motion1(); 
        commandReceived = true;
      } else if (val == N2) {
        Serial.println(">>> BUTTON 2 MATCHED! Starting Motion 2 <<<");
        Motion2(); 
        commandReceived = true;
      } else {
        Serial.println(">>> Button pressed but doesn't match expected codes <<<");
        Serial.println("If this is button 1 or 2, update the code with this value:");
        Serial.print("For button 1: #define N1 ");
        Serial.println(val);
        Serial.print("For button 2: #define N2 ");
        Serial.println(val);
        Serial.println("Continuing to listen...");
      }
      delay(1000); // Prevent multiple rapid readings
    }
    delay(100);
  }
  
  if (!commandReceived) {
    Serial.println("TIMEOUT - No valid command received in 60 seconds");
    // Rapid blink to indicate timeout
    for (int i = 0; i < 20; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
    }
  }
}

void loop() {
  // Continue listening for more commands after first one completes
  if (irrecv.decode(&results)) {
    val = results.value;
    Serial.print("Additional IR code: ");
    Serial.println(val);
    
    // Quick blink for feedback
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    
    irrecv.resume();
    delay(500);
  }
  delay(100);
}
