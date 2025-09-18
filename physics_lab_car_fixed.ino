// Added Arduino core include for intellisense
#include <Arduino.h>
/*
  Physics Lab Car - Dual Mode Line Follower with Stop Mark
  Floor: grey, Line: black, Stop mark: light (white) tape

  Modes (pin 12 with INPUT_PULLUP):
    - Pin 12 HIGH (disconnected): Mode 2 -> After white mark: continue forward SLOW for up to AFTER_MARK_TIMEOUT_MS (then halt)
    - Pin 12 LOW  (jumper to GND): Mode 1 -> After white mark: brief pause then reverse for REVERSE_DURATION_MS (capped by timeout) then halt

  LED (pin 13 / built-in):
    - Solid ON during Mode 1 reversing phase
    - Blinks slowly (1s period) while in Mode 2 slow-forward phase
    - Off while line following; fast triple blink at startup shows ready

  Adjust constants below for speeds, durations, timeout.
*/

// Motor pins
const int ENA = 5; // Left speed
const int ENB = 6; // Right speed
const int IN1 = 7; // Left forward
const int IN2 = 8; // Left reverse
const int IN3 = 9; // Right reverse
const int IN4 = 11; // Right forward

// Line sensors (digital)
const int LT_L = 2;
const int LT_M = 4;
const int LT_R = 10;

// Mode select pin
const int MODE_SELECT_PIN = 12; // LOW = Mode 1 (reverse after mark); HIGH = Mode 2 (slow forward)

// LED
const int STATUS_LED = 13; // Built-in LED

// Speeds
const int SPEED_FORWARD = 150; // Line following base speed
const int SPEED_TURN_DELTA = 60; // Reduction when correcting
const int SPEED_SLOW = 90;      // Slow forward after mark (Mode 2)
const int SPEED_REVERSE = 110;  // Reverse speed (Mode 1)

// Timing
const unsigned long REVERSE_DURATION_MS = 3000;       // Max reverse duration (Mode 1)
const unsigned long AFTER_MARK_TIMEOUT_MS = 5000;     // Post-mark overall movement timeout
const unsigned long PRE_REVERSE_PAUSE_MS = 200;       // Brief pause before reversing

// State machine
enum RunState { FOLLOW_LINE, MARK_DETECTED, REVERSING, SLOW_FORWARD, HALT };
RunState runState = FOLLOW_LINE;

// Time markers
unsigned long markDetectedMillis = 0;
unsigned long slowModeStartMillis = 0;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LT_L, INPUT);
  pinMode(LT_M, INPUT);
  pinMode(LT_R, INPUT);

  pinMode(MODE_SELECT_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);

  stopMotors();
  // Startup triple blink
  for (int i=0;i<3;i++){digitalWrite(STATUS_LED,HIGH);delay(150);digitalWrite(STATUS_LED,LOW);delay(150);}  
}

void loop() {
  bool modeReverse = (digitalRead(MODE_SELECT_PIN) == LOW);

  // Sensor reads (LOW = black line, HIGH = floor/tape)
  int left = digitalRead(LT_L);
  int middle = digitalRead(LT_M);
  int right = digitalRead(LT_R);

  switch (runState) {
    case FOLLOW_LINE: {
      digitalWrite(STATUS_LED, LOW);
      if (middle == HIGH) { // hit white/light tape
        markDetectedMillis = millis();
        if (modeReverse) {
          runState = MARK_DETECTED;
        } else {
          slowModeStartMillis = millis();
          runState = SLOW_FORWARD;
        }
        break;
      }
      // Line follow corrections
      if (middle == LOW) {
        forward(SPEED_FORWARD, SPEED_FORWARD);
      } else if (left == LOW) {
        forward(SPEED_FORWARD - SPEED_TURN_DELTA, SPEED_FORWARD);
      } else if (right == LOW) {
        forward(SPEED_FORWARD, SPEED_FORWARD - SPEED_TURN_DELTA);
      } else {
        stopMotors();
      }
      break;
    }
    case MARK_DETECTED: {
      stopMotors();
      if (millis() - markDetectedMillis >= PRE_REVERSE_PAUSE_MS) {
        runState = REVERSING;
      }
      break;
    }
    case REVERSING: {
      unsigned long elapsed = millis() - markDetectedMillis;
      digitalWrite(STATUS_LED, HIGH); // Solid during reversing
      if (elapsed <= REVERSE_DURATION_MS && elapsed <= AFTER_MARK_TIMEOUT_MS) {
        // Experimental reverse line tracking: sensors still face forward, so steering is inverted.
        // Goal: keep middle sensor over black (LOW). Adjust by biasing opposite wheel speeds.
        if (middle == LOW) {
          reverse(SPEED_REVERSE, SPEED_REVERSE);
        } else if (left == LOW) {
          // Line is under left sensor: car likely drifted left; while reversing we slow left wheel to pivot back over line.
          reverse(SPEED_REVERSE - (SPEED_TURN_DELTA/2), SPEED_REVERSE);
        } else if (right == LOW) {
          reverse(SPEED_REVERSE, SPEED_REVERSE - (SPEED_TURN_DELTA/2));
        } else {
          // Lost line (all HIGH) - gentle stop to avoid wandering blindly.
            stopMotors();
        }
      } else {
        stopMotors();
        runState = HALT;
      }
      break;
    }
    case SLOW_FORWARD: {
      unsigned long elapsed = millis() - slowModeStartMillis;
      // Slow blink: on 500ms, off 500ms (approx 1s period)
      digitalWrite(STATUS_LED, (millis() / 500) % 2 ? HIGH : LOW);
      if (elapsed <= AFTER_MARK_TIMEOUT_MS) {
        // Continue line following but with reduced speed & gentler corrections
        if (middle == LOW) {
          forward(SPEED_SLOW, SPEED_SLOW);
        } else if (left == LOW) {
          forward(SPEED_SLOW - (SPEED_TURN_DELTA/2), SPEED_SLOW);
        } else if (right == LOW) {
          forward(SPEED_SLOW, SPEED_SLOW - (SPEED_TURN_DELTA/2));
        } else {
          // Lost line during slow phase: stop to avoid wandering
          stopMotors();
        }
      } else {
        stopMotors();
        digitalWrite(STATUS_LED, LOW);
        runState = HALT;
      }
      break;
    }
    case HALT: {
      stopMotors();
      digitalWrite(STATUS_LED, LOW);
      // Remain halted
      break;
    }
  }
}

// Motor helpers
void forward(int leftSpeed, int rightSpeed) {
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void reverse(int leftSpeed, int rightSpeed) {
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
