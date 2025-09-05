/*
  Physics Lab Arduino Car - Average Velocity Experiment
  
  Features:
  - Two motion modes selectable via IR remote (1 and 2)
  - Motion correction for straight line movement
  - Visual and audio feedback for user interaction
  - Precise distance control using encoders or time-based movement
  
  Motion 1: Forward 60cm, then backward 40cm (different speeds)
  Motion 2: Forward 70cm, then backward 70cm (different speeds)
*/

#include <IRremote.h>

// Pin definitions
const int MOTOR_LEFT_FORWARD = 5;   // Left motor forward
const int MOTOR_LEFT_BACKWARD = 6;  // Left motor backward
const int MOTOR_RIGHT_FORWARD = 9;  // Right motor forward
const int MOTOR_RIGHT_BACKWARD = 10; // Right motor backward
const int IR_RECEIVER_PIN = 11;     // IR receiver pin
const int LED_PIN = 13;             // Built-in LED for status
const int BUZZER_PIN = 8;           // Buzzer for audio feedback

// IR remote codes (adjust these based on your remote)
const unsigned long REMOTE_KEY_1 = 0xFF30CF;
const unsigned long REMOTE_KEY_2 = 0xFF18E7;

// Motor calibration values (adjust these to make car go straight)
int LEFT_MOTOR_CORRECTION = 0;   // -20 to +20 adjustment for left motor
int RIGHT_MOTOR_CORRECTION = 0;  // -20 to +20 adjustment for right motor

// Distance and timing constants
const float CM_PER_SECOND_SLOW = 15.0;   // Slow speed: 15 cm/sec
const float CM_PER_SECOND_FAST = 25.0;   // Fast speed: 25 cm/sec

// Motion parameters
struct Motion {
  float forward_distance;
  float backward_distance;
  float forward_speed;
  float backward_speed;
};

Motion motion1 = {60.0, 40.0, CM_PER_SECOND_SLOW, CM_PER_SECOND_FAST};
Motion motion2 = {70.0, 70.0, CM_PER_SECOND_FAST, CM_PER_SECOND_SLOW};

// Global variables
int selected_motion = 0;
bool motion_ready = false;
IRrecv irrecv(IR_RECEIVER_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  Serial.println("Physics Lab Car - Average Velocity Experiment");
  Serial.println("Ready for motion selection...");
  
  // Initialize pins
  pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize IR receiver
  irrecv.enableIRIn();
  
  // Initial status indication
  statusReady();
  
  Serial.println("Press '1' for Motion 1 (60cm forward, 40cm back)");
  Serial.println("Press '2' for Motion 2 (70cm forward, 70cm back)");
}

void loop() {
  // Check for IR remote input
  if (irrecv.decode(&results)) {
    handleRemoteInput(results.value);
    irrecv.resume();
  }
  
  // Execute selected motion
  if (motion_ready && selected_motion > 0) {
    executeMotion();
    motion_ready = false;
    selected_motion = 0;
    
    // Return to ready state
    delay(2000);
    statusReady();
    Serial.println("Ready for next motion selection...");
  }
  
  delay(100);
}

void handleRemoteInput(unsigned long code) {
  Serial.print("Received IR code: 0x");
  Serial.println(code, HEX);
  
  if (code == REMOTE_KEY_1) {
    selected_motion = 1;
    Serial.println("Motion 1 selected: 60cm forward, 40cm backward");
    motionSelected();
  } else if (code == REMOTE_KEY_2) {
    selected_motion = 2;
    Serial.println("Motion 2 selected: 70cm forward, 70cm backward");
    motionSelected();
  }
}

void motionSelected() {
  // Visual confirmation
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  
  Serial.println("Motion selected! Starting countdown...");
  countdown();
  motion_ready = true;
}

void countdown() {
  for (int i = 3; i > 0; i--) {
    Serial.print("Starting in ");
    Serial.print(i);
    Serial.println("...");
    
    // Visual and audio countdown
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 800, 500);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  
  // Start signal
  Serial.println("GO!");
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1500, 1000);
  delay(500);
  digitalWrite(LED_PIN, LOW);
}

void executeMotion() {
  Motion current_motion;
  
  if (selected_motion == 1) {
    current_motion = motion1;
  } else {
    current_motion = motion2;
  }
  
  Serial.println("Starting motion...");
  
  // Forward motion
  Serial.print("Moving forward ");
  Serial.print(current_motion.forward_distance);
  Serial.println(" cm");
  
  moveForward(current_motion.forward_distance, current_motion.forward_speed);
  
  // Brief pause between directions
  stopMotors();
  delay(500);
  
  // Backward motion
  Serial.print("Moving backward ");
  Serial.print(current_motion.backward_distance);
  Serial.println(" cm");
  
  moveBackward(current_motion.backward_distance, current_motion.backward_speed);
  
  // Stop and completion signal
  stopMotors();
  Serial.println("Motion complete!");
  
  // Completion signal
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 2000, 100);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void moveForward(float distance_cm, float speed_cm_per_sec) {
  int duration_ms = (distance_cm / speed_cm_per_sec) * 1000;
  int motor_speed = mapSpeedToMotorPWM(speed_cm_per_sec);
  
  // Apply motor correction for straight movement
  int left_speed = constrain(motor_speed + LEFT_MOTOR_CORRECTION, 0, 255);
  int right_speed = constrain(motor_speed + RIGHT_MOTOR_CORRECTION, 0, 255);
  
  analogWrite(MOTOR_LEFT_FORWARD, left_speed);
  analogWrite(MOTOR_RIGHT_FORWARD, right_speed);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
  
  delay(duration_ms);
  stopMotors();
}

void moveBackward(float distance_cm, float speed_cm_per_sec) {
  int duration_ms = (distance_cm / speed_cm_per_sec) * 1000;
  int motor_speed = mapSpeedToMotorPWM(speed_cm_per_sec);
  
  // Apply motor correction for straight movement
  int left_speed = constrain(motor_speed + LEFT_MOTOR_CORRECTION, 0, 255);
  int right_speed = constrain(motor_speed + RIGHT_MOTOR_CORRECTION, 0, 255);
  
  analogWrite(MOTOR_LEFT_BACKWARD, left_speed);
  analogWrite(MOTOR_RIGHT_BACKWARD, right_speed);
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  
  delay(duration_ms);
  stopMotors();
}

void stopMotors() {
  analogWrite(MOTOR_LEFT_FORWARD, 0);
  analogWrite(MOTOR_LEFT_BACKWARD, 0);
  analogWrite(MOTOR_RIGHT_FORWARD, 0);
  analogWrite(MOTOR_RIGHT_BACKWARD, 0);
}

int mapSpeedToMotorPWM(float speed_cm_per_sec) {
  // Map speed in cm/sec to PWM value (0-255)
  // This mapping may need adjustment based on your specific motors
  if (speed_cm_per_sec <= 10.0) return 120;
  else if (speed_cm_per_sec <= 15.0) return 160;
  else if (speed_cm_per_sec <= 20.0) return 200;
  else if (speed_cm_per_sec <= 25.0) return 240;
  else return 255;
}

void statusReady() {
  // Slow blinking LED to indicate ready state
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
  
  // Ready tone
  tone(BUZZER_PIN, 500, 300);
  delay(400);
  tone(BUZZER_PIN, 700, 300);
}

// Calibration functions - call these to adjust straight-line movement
void calibrateLeft(int adjustment) {
  LEFT_MOTOR_CORRECTION = constrain(adjustment, -20, 20);
  Serial.print("Left motor correction set to: ");
  Serial.println(LEFT_MOTOR_CORRECTION);
}

void calibrateRight(int adjustment) {
  RIGHT_MOTOR_CORRECTION = constrain(adjustment, -20, 20);
  Serial.print("Right motor correction set to: ");
  Serial.println(RIGHT_MOTOR_CORRECTION);
}

// Test function for calibration
void testStraightMovement() {
  Serial.println("Testing straight movement for 2 seconds...");
  moveForward(30, 15); // Move forward 30cm at slow speed
  delay(1000);
  moveBackward(30, 15); // Move backward 30cm at slow speed
  Serial.println("Test complete. Adjust calibration if needed.");
}
