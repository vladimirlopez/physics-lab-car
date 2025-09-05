/*
  Simple Physics Lab Car - Straight Line Motion
  
  This sketch will:
  1. Move forward 50cm at slow speed (constant velocity)
  2. Move forward 50cm more at fast speed (constant velocity) 
  3. Stop
  
  Uses the same pin configuration as your working sketch.
  No IR remote needed - automatic execution after startup.
*/

// Pin definitions (from your working Average_Velocity_Lab_Final.ino)
#define ENB 5          // Left wheel speed
#define IN1 7          // Left wheel forward  
#define IN2 8          // Left wheel reverse
#define IN3 9          // Right wheel reverse
#define IN4 11         // Right wheel forward
#define ENA 6          // Right wheel speed

// Speed definitions
#define SLOW_SPEED 120   // Slow constant velocity (~15 cm/sec)
#define FAST_SPEED 200   // Fast constant velocity (~25 cm/sec)

// Motor calibration for straight movement (adjust these if car drifts)
int LEFT_MOTOR_CORRECTION = 0;   // -20 to +20 adjustment
int RIGHT_MOTOR_CORRECTION = 0;  // -20 to +20 adjustment

// Distance timing calculations
// For 50cm at 15 cm/sec: 50/15 = 3.33 seconds = 3333ms
// For 50cm at 25 cm/sec: 50/25 = 2.0 seconds = 2000ms
#define TIME_50CM_SLOW 3333   // Time for 50cm at slow speed
#define TIME_50CM_FAST 2000   // Time for 50cm at fast speed

void forward(int selectedSpeed) {
  // Apply motor correction for straight movement
  int leftSpeed = constrain(selectedSpeed + LEFT_MOTOR_CORRECTION, 0, 255);
  int rightSpeed = constrain(selectedSpeed + RIGHT_MOTOR_CORRECTION, 0, 255);
  
  analogWrite(ENA, rightSpeed);
  analogWrite(ENB, leftSpeed);
  digitalWrite(IN1, HIGH);   // Left wheel forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);   // Right wheel forward
  
  Serial.print("Moving forward at speed ");
  Serial.println(selectedSpeed);
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
  Serial.println("STOP!");
}

void readySignal() {
  Serial.println("=== PHYSICS LAB CAR - STRAIGHT LINE MOTION ===");
  Serial.println("Motion plan:");
  Serial.println("1. Forward 50cm at slow speed (constant velocity)");
  Serial.println("2. Forward 50cm at fast speed (constant velocity)");
  Serial.println("3. Stop");
  Serial.println("============================================");
  
  // 3 bright blinks to indicate ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
  }
  
  Serial.println("Ready! Starting in 3 seconds...");
}

void countdown() {
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

void executeMotion() {
  Serial.println("=== MOTION STARTING ===");
  
  // Phase 1: Forward 50cm at slow speed (constant velocity)
  Serial.println("Phase 1: Moving forward 50cm at SLOW speed (constant velocity)");
  Serial.print("Speed: ");
  Serial.print(SLOW_SPEED);
  Serial.print(" PWM, Duration: ");
  Serial.print(TIME_50CM_SLOW);
  Serial.println(" ms");
  
  forward(SLOW_SPEED);
  delay(TIME_50CM_SLOW);
  stop();
  
  // Brief pause between phases
  Serial.println("Brief pause between phases...");
  delay(1000);
  
  // Phase 2: Forward 50cm more at fast speed (constant velocity)
  Serial.println("Phase 2: Moving forward 50cm more at FAST speed (constant velocity)");
  Serial.print("Speed: ");
  Serial.print(FAST_SPEED);
  Serial.print(" PWM, Duration: ");
  Serial.print(TIME_50CM_FAST);
  Serial.println(" ms");
  
  forward(FAST_SPEED);
  delay(TIME_50CM_FAST);
  stop();
  
  Serial.println("=== MOTION COMPLETE ===");
  Serial.println("Total distance: 100cm (50cm slow + 50cm fast)");
  
  // Completion signal - rapid blinking
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
  
  // Make sure motors are stopped initially
  stop();
  
  // Ready signal and countdown
  readySignal();
  countdown();
  
  // Execute the motion
  executeMotion();
  
  Serial.println("Program complete. Reset Arduino to run again.");
}

void loop() {
  // Slow blink to indicate program is complete
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  
  // Optional: Uncomment the line below to automatically restart after 30 seconds
  // if (millis() > 30000) { setup(); }
}
