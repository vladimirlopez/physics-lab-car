/*
  ALTERNATING MOTOR SPEEDS - Software-Only Straight Line Solution
  
  This version uses alternating motor corrections to average out 
  construction differences over the distance traveled.
  
  NO SENSORS NEEDED - works on any surface!
  Strategy: Rapidly alternate slight corrections to average out drift
*/

// Pin definitions
#define ENB 5          // Left wheel speed
#define IN1 7          // Left wheel forward  
#define IN2 8          // Left wheel reverse
#define IN3 9          // Right wheel reverse
#define IN4 11         // Right wheel forward
#define ENA 6          // Right wheel speed

// Speed definitions
#define SLOW_SPEED 120   
#define FAST_SPEED 200   

// Correction strategy parameters
#define CORRECTION_INTERVAL 100  // Switch correction every 100ms
#define CORRECTION_AMOUNT 15     // Amount of correction to apply

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  // Initialize pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  stop();
  
  Serial.println("=== ALTERNATING CORRECTION CAR ===");
  Serial.println("Uses software corrections to go straight");
  Serial.println("No sensors or calibration needed!");
  Serial.println("==================================");
  
  readySignal();
  executeMotion();
}

void readySignal() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
  
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
}

void forwardWithAlternatingCorrection(int baseSpeed, unsigned long duration) {
  unsigned long startTime = millis();
  bool correctLeft = true; // Start with left correction
  unsigned long lastSwitch = millis();
  
  Serial.print("Moving forward with alternating corrections, speed ");
  Serial.print(baseSpeed);
  Serial.print(", duration ");
  Serial.print(duration);
  Serial.println(" ms");
  
  while (millis() - startTime < duration) {
    // Switch correction direction periodically
    if (millis() - lastSwitch >= CORRECTION_INTERVAL) {
      correctLeft = !correctLeft;
      lastSwitch = millis();
    }
    
    // Apply alternating correction
    int leftSpeed, rightSpeed;
    if (correctLeft) {
      leftSpeed = baseSpeed + CORRECTION_AMOUNT;
      rightSpeed = baseSpeed - CORRECTION_AMOUNT;
    } else {
      leftSpeed = baseSpeed - CORRECTION_AMOUNT;
      rightSpeed = baseSpeed + CORRECTION_AMOUNT;
    }
    
    // Move forward with current correction
    forward(leftSpeed, rightSpeed);
    
    delay(10); // Small delay for stability
  }
  
  stop();
}

void forward(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, 50, 255);
  rightSpeed = constrain(rightSpeed, 50, 255);
  
  analogWrite(ENB, leftSpeed);   // Left wheel
  analogWrite(ENA, rightSpeed);  // Right wheel
  digitalWrite(IN1, HIGH);       // Left forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);       // Right forward
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void executeMotion() {
  Serial.println("=== MOTION STARTING ===");
  
  // Phase 1: 50cm slow with alternating corrections
  Serial.println("Phase 1: 50cm SLOW with alternating corrections");
  forwardWithAlternatingCorrection(SLOW_SPEED, 3333);
  
  Serial.println("Brief pause...");
  delay(1000);
  
  // Phase 2: 50cm fast with alternating corrections
  Serial.println("Phase 2: 50cm FAST with alternating corrections");
  forwardWithAlternatingCorrection(FAST_SPEED, 2000);
  
  Serial.println("=== MOTION COMPLETE ===");
  
  // Completion signal
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
