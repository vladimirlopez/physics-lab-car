/*
  DIAGNOSTIC VERSION - Physics Lab Arduino Car
  
  This version will help identify issues with:
  - IR remote reception
  - Motor control
  - Audio/Visual feedback
  - Pin connections
*/

#include <IRremote.h>

// Pin definitions
const int MOTOR_LEFT_FORWARD = 5;   
const int MOTOR_LEFT_BACKWARD = 6;  
const int MOTOR_RIGHT_FORWARD = 9;  
const int MOTOR_RIGHT_BACKWARD = 10; 
const int IR_RECEIVER_PIN = 11;     
const int LED_PIN = 13;             
const int BUZZER_PIN = 8;           

// Global variables for IR
IRrecv irrecv(IR_RECEIVER_PIN);
decode_results results;

// Diagnostic flags
bool ir_working = false;
bool buzzer_working = false;
bool led_working = false;

void setup() {
  Serial.begin(9600);
  delay(2000); // Give time for serial monitor to connect
  
  Serial.println("=================================");
  Serial.println("DIAGNOSTIC MODE - Physics Lab Car");
  Serial.println("=================================");
  Serial.println();
  
  // Initialize pins
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Test LED
  Serial.println("1. Testing LED...");
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  Serial.println("   LED test complete - Did you see blinking?");
  led_working = true;
  
  // Test Buzzer
  Serial.println("2. Testing Buzzer...");
  for (int freq = 500; freq <= 2000; freq += 500) {
    tone(BUZZER_PIN, freq, 300);
    delay(400);
  }
  Serial.println("   Buzzer test complete - Did you hear tones?");
  buzzer_working = true;
  
  // Test Motors (brief pulse)
  Serial.println("3. Testing Motors...");
  Serial.println("   Left motor forward...");
  analogWrite(MOTOR_LEFT_FORWARD, 100);
  delay(500);
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  delay(500);
  
  Serial.println("   Right motor forward...");
  analogWrite(MOTOR_RIGHT_FORWARD, 100);
  delay(500);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  delay(500);
  
  Serial.println("   Motor test complete - Did motors turn?");
  
  // Initialize IR receiver
  Serial.println("4. Initializing IR Receiver...");
  irrecv.enableIRIn();
  Serial.println("   IR Receiver initialized on pin 11");
  
  // Final status
  Serial.println();
  Serial.println("=== DIAGNOSTIC COMPLETE ===");
  Serial.println("Now point your IR remote at the car and press ANY button");
  Serial.println("IR codes will be displayed below:");
  Serial.println();
}

void loop() {
  // Check for ANY IR input and display the codes
  if (irrecv.decode(&results)) {
    ir_working = true;
    
    Serial.print("IR RECEIVED! Code: 0x");
    Serial.print(results.value, HEX);
    Serial.print(" (");
    Serial.print(results.value);
    Serial.print(") Protocol: ");
    
    // Check protocol type (for newer IRremote library versions)
    #ifdef DECODE_NEC
      if (results.decode_type == NEC) Serial.print("NEC");
      else if (results.decode_type == SONY) Serial.print("SONY");  
      else if (results.decode_type == RC5) Serial.print("RC5");
      else if (results.decode_type == RC6) Serial.print("RC6");
      else Serial.print("UNKNOWN");
    #endif
    
    Serial.println();
    
    // Give feedback for ANY button press
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    
    // Test specific codes (you'll need to update these)
    if (results.value == 0xFF30CF) {
      Serial.println(">>> BUTTON 1 DETECTED! (Motion 1) <<<");
      testMotion1();
    } else if (results.value == 0xFF18E7) {
      Serial.println(">>> BUTTON 2 DETECTED! (Motion 2) <<<");
      testMotion2();
    } else {
      Serial.println(">>> Unknown button - Update code with this value <<<");
    }
    
    irrecv.resume(); // Continue listening
  }
  
  // Periodic status report
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 10000) { // Every 10 seconds
    lastStatus = millis();
    Serial.println();
    Serial.println("--- STATUS REPORT ---");
    Serial.print("IR Working: "); Serial.println(ir_working ? "YES" : "NO - Check connections");
    Serial.print("LED Working: "); Serial.println(led_working ? "YES" : "NO");
    Serial.print("Buzzer Working: "); Serial.println(buzzer_working ? "YES" : "NO");
    Serial.println("Waiting for IR input...");
    Serial.println();
  }
  
  delay(100);
}

void testMotion1() {
  Serial.println("TESTING MOTION 1: 60cm forward, 40cm back");
  
  // Visual countdown
  for (int i = 3; i > 0; i--) {
    Serial.print("Starting in ");
    Serial.print(i);
    Serial.println("...");
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 800, 500);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  
  Serial.println("GO!");
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1500, 500);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  
  // Forward motion (2 seconds at slow speed)
  Serial.println("Moving forward...");
  analogWrite(MOTOR_LEFT_FORWARD, 160);
  analogWrite(MOTOR_RIGHT_FORWARD, 160);
  delay(4000); // 4 seconds for 60cm
  
  // Stop
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  delay(500);
  
  // Backward motion (1.6 seconds at fast speed) 
  Serial.println("Moving backward...");
  analogWrite(MOTOR_LEFT_BACKWARD, 240);
  analogWrite(MOTOR_RIGHT_BACKWARD, 240);
  delay(1600); // 1.6 seconds for 40cm
  
  // Stop
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
  
  Serial.println("Motion 1 complete!");
  
  // Completion signal
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 2000, 100);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void testMotion2() {
  Serial.println("TESTING MOTION 2: 70cm forward, 70cm back");
  
  // Visual countdown
  for (int i = 3; i > 0; i--) {
    Serial.print("Starting in ");
    Serial.print(i);
    Serial.println("...");
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 800, 500);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  
  Serial.println("GO!");
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1500, 500);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  
  // Forward motion (2.8 seconds at fast speed)
  Serial.println("Moving forward...");
  analogWrite(MOTOR_LEFT_FORWARD, 240);
  analogWrite(MOTOR_RIGHT_FORWARD, 240);
  delay(2800); // 2.8 seconds for 70cm
  
  // Stop
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  delay(500);
  
  // Backward motion (4.67 seconds at slow speed)
  Serial.println("Moving backward...");
  analogWrite(MOTOR_LEFT_BACKWARD, 160);
  analogWrite(MOTOR_RIGHT_BACKWARD, 160);
  delay(4670); // 4.67 seconds for 70cm
  
  // Stop
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
  
  Serial.println("Motion 2 complete!");
  
  // Completion signal
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 2000, 100);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}
