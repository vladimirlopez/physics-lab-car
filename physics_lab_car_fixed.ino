/*
  Physics Lab Arduino Car - Fixed Version Based on Working Sketch
  
  Corrected based on your working Average_Velocity_Lab_Final.ino
  - Uses correct pin assignments from your working sketch
  - Uses your remote control codes  
  - Simplified IR handling
  - Two motion modes for physics lab
  
  Motion 1: Press '1' - Forward 60cm, then backward 40cm (different speeds)
  Motion 2: Press '2' - Forward 70cm, then backward 70cm (different speeds)
*/

#include <IRremote.h>

// Remote control codes (from your working sketch)
#define N1 16738455    // number 1
#define N2 16750695    // number 2
#define N3 16756815    // number 3
#define N4 16724175    // number 4
#define N5 16718055    // number 5
#define N6 16743045    // number 6
#define N7 16716015    // number 7
#define N8 16726215    // number 8

// Pin definitions (from your working sketch)
#define RECV_PIN 12    // IR receiver pin (was 12 in working sketch, not 11)

// Motor pin definitions (from your working sketch)
#define ENB 5          // Left wheel speed
#define IN1 7          // Left wheel forward  
#define IN2 8          // Left wheel reverse
#define IN3 9          // Right wheel reverse
#define IN4 11         // Right wheel forward
#define ENA 6          // Right wheel speed

// Speed definitions
#define SLOW_SPEED 120   // Slow speed (15 cm/sec equivalent)
#define FAST_SPEED 250   // Fast speed (25 cm/sec equivalent)

// Distance timing (in milliseconds) - adjust these based on testing
#define TIME_60CM_SLOW 4000   // Time for 60cm at slow speed
#define TIME_40CM_FAST 1600   // Time for 40cm at fast speed  
#define TIME_70CM_SLOW 4667   // Time for 70cm at slow speed
#define TIME_70CM_FAST 2800   // Time for 70cm at fast speed

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

// Signal functions for user feedback
void readySignal() {
  Serial.println("=== PHYSICS LAB CAR READY ===");
  Serial.println("Press '1' for Motion 1 (60cm forward, 40cm back)");
  Serial.println("Press '2' for Motion 2 (70cm forward, 70cm back)");
  
  // Visual signal - 3 bright blinks
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
  // Ready signal - dimmer light for 2 seconds
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
}

void selectionConfirmed(int motionNumber) {
  Serial.print("Motion ");
  Serial.print(motionNumber);
  Serial.println(" selected! Starting countdown...");
  
  // Quick confirmation blinks
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
  
  Serial.println("GO!");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

void motionComplete() {
  Serial.println("Motion complete!");
  
  // Completion signal - 5 quick blinks
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

// Motion 1: Forward 60cm (slow), then backward 40cm (fast)
void Motion1() {
  selectionConfirmed(1);
  
  Serial.println("Phase 1: Moving forward 60cm at slow speed");
  forward(SLOW_SPEED);
  delay(TIME_60CM_SLOW);
  stop();
  delay(500); // Brief pause
  
  Serial.println("Phase 2: Moving backward 40cm at fast speed");
  back(FAST_SPEED);
  delay(TIME_40CM_FAST);
  stop();
  
  motionComplete();
}

// Motion 2: Forward 70cm (fast), then backward 70cm (slow)
void Motion2() {
  selectionConfirmed(2);
  
  Serial.println("Phase 1: Moving forward 70cm at fast speed");
  forward(FAST_SPEED);
  delay(TIME_70CM_FAST);
  stop();
  delay(500); // Brief pause
  
  Serial.println("Phase 2: Moving backward 70cm at slow speed");
  back(SLOW_SPEED);
  delay(TIME_70CM_SLOW);
  stop();
  
  motionComplete();
}

void setup() {
  Serial.begin(9600);
  delay(1000); // Give serial monitor time to connect
  
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
  
  // Wait for IR command (timeout after 30 seconds)
  unsigned long startTime = millis();
  bool commandReceived = false;
  
  Serial.println("Waiting for remote control input...");
  
  while (!commandReceived && (millis() - startTime < 30000)) {
    if (irrecv.decode(&results)) {
      val = results.value;
      Serial.print("IR Code received: ");
      Serial.println(val);
      irrecv.resume();
      
      switch(val) {
        case N1: 
          Motion1(); 
          commandReceived = true;
          break;
        case N2: 
          Motion2(); 
          commandReceived = true;
          break;
        case N3:
        case N4:
        case N5:
        case N6:
        case N7:
        case N8:
          Serial.println("Button recognized but not programmed. Use '1' or '2'.");
          break;
        default:
          Serial.print("Unknown button pressed. Code: ");
          Serial.println(val);
          Serial.println("Use buttons '1' or '2' for motion selection.");
          break;
      }
      delay(500); // Debounce delay
    }
    delay(100);
  }
  
  if (!commandReceived) {
    Serial.println("Timeout - no valid command received");
    // Blink rapidly to indicate timeout
    for (int i = 0; i < 10; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
    }
  }
}

void loop() {
  // Main program runs once in setup(), loop() does nothing
  delay(1000);
}
