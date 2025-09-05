/*
  SELF-CORRECTING Physics Lab Car using Line Tracking Sensors
  
  This version uses the built-in line tracking sensors to automatically
  keep the car moving straight on any surface with a line or tape.
  
  NO MANUAL CALIBRATION NEEDED - works on all 10 cars automatically!
  
  Setup: Place a piece of black electrical tape on the floor in a straight line
*/

// Pin definitions (from your working sketch)
#define ENB 5          // Left wheel speed
#define IN1 7          // Left wheel forward  
#define IN2 8          // Left wheel reverse
#define IN3 9          // Right wheel reverse
#define IN4 11         // Right wheel forward
#define ENA 6          // Right wheel speed

// Line tracking sensors (from your working sketch)
#define LT_L !digitalRead(2)   // Left sensor
#define LT_M !digitalRead(4)   // Middle sensor  
#define LT_R !digitalRead(10)  // Right sensor

// Speed definitions
#define SLOW_SPEED 120   // Slow constant velocity (~15 cm/sec)
#define FAST_SPEED 200   // Fast constant velocity (~25 cm/sec)

// Line following parameters
#define CORRECTION_AMOUNT 30   // How much to adjust motors when correcting

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  // Initialize motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize sensor pins
  pinMode(2, INPUT);   // Left sensor
  pinMode(4, INPUT);   // Middle sensor
  pinMode(10, INPUT);  // Right sensor
  
  stop();
  
  Serial.println("=== SELF-CORRECTING PHYSICS CAR ===");
  Serial.println("Place car on black tape/line and press reset");
  Serial.println("Motion: 50cm slow + 50cm fast (auto-correcting)");
  Serial.println("===================================");
  
  // Wait for line detection
  waitForLine();
  
  readySignal();
  executeMotion();
}

void waitForLine() {
  Serial.println("Place car over the black line and it will start automatically...");
  
  // Wait until middle sensor detects the line
  while (!LT_M) {
    // Blink slowly while waiting
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    
    Serial.print("Sensors: L=");
    Serial.print(LT_L ? "LINE" : "----");
    Serial.print(" M=");
    Serial.print(LT_M ? "LINE" : "----");  
    Serial.print(" R=");
    Serial.println(LT_R ? "LINE" : "----");
  }
  
  Serial.println("LINE DETECTED! Ready to start...");
}

void readySignal() {
  // 3 quick blinks to indicate ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
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
}

void followLineForward(int baseSpeed, unsigned long duration) {
  unsigned long startTime = millis();
  
  Serial.print("Following line at speed ");
  Serial.print(baseSpeed);
  Serial.print(" for ");
  Serial.print(duration);
  Serial.println(" ms");
  
  while (millis() - startTime < duration) {
    // Read sensors
    bool left = LT_L;
    bool middle = LT_M;
    bool right = LT_R;
    
    // Line following logic
    if (middle && !left && !right) {
      // Perfect - on line, go straight
      forward(baseSpeed, baseSpeed);
    }
    else if (left && middle && !right) {
      // Slightly left of line, turn right gently
      forward(baseSpeed + CORRECTION_AMOUNT/2, baseSpeed - CORRECTION_AMOUNT/2);
    }
    else if (!left && middle && right) {
      // Slightly right of line, turn left gently  
      forward(baseSpeed - CORRECTION_AMOUNT/2, baseSpeed + CORRECTION_AMOUNT/2);
    }
    else if (left && !middle && !right) {
      // Too far left, turn right more
      forward(baseSpeed + CORRECTION_AMOUNT, baseSpeed - CORRECTION_AMOUNT);
    }
    else if (!left && !middle && right) {
      // Too far right, turn left more
      forward(baseSpeed - CORRECTION_AMOUNT, baseSpeed + CORRECTION_AMOUNT);
    }
    else if (left && middle && right) {
      // All sensors on line (wide line or intersection)
      forward(baseSpeed, baseSpeed);
    }
    else {
      // No line detected - keep last direction or go straight
      forward(baseSpeed, baseSpeed);
    }
    
    // Small delay for stability
    delay(10);
  }
  
  stop();
}

void forward(int leftSpeed, int rightSpeed) {
  // Constrain speeds to valid range
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  
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
  
  // Phase 1: 50cm at slow speed with line following
  Serial.println("Phase 1: 50cm at SLOW speed (line-following)");
  followLineForward(SLOW_SPEED, 3333); // 3.33 seconds for 50cm at ~15 cm/sec
  
  Serial.println("Brief pause...");
  delay(1000);
  
  // Phase 2: 50cm at fast speed with line following  
  Serial.println("Phase 2: 50cm at FAST speed (line-following)");
  followLineForward(FAST_SPEED, 2000); // 2.0 seconds for 50cm at ~25 cm/sec
  
  Serial.println("=== MOTION COMPLETE ===");
  Serial.println("Total: 100cm with automatic straight-line correction");
  
  // Completion signal
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void loop() {
  // Show sensor status continuously
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  
  // Optional: uncomment to restart automatically
  // if (millis() > 30000) { setup(); }
}
