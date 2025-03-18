/* 
 * Example sketch to control two stepper motors with
 * A4988/DRV8825 stepper motor driver and
 * Arduino without a library to move in x-y axis
 * 
 * Created by NIHAL T P
 * GitHub: nihaltp
*/

#include <Arduino.h>

// Define stepper motor connections and steps per revolution:
#define dirPinX  2  // Direction Pin
#define stepPinX 3  // Step Pin

#define dirPinY  4  // Direction Pin
#define stepPinY 5  // Step Pin

#define stepsPerRevolution 200

void square(int delayTime, int repeat);
void diagonal(int delayTime, int repeat);
void runStepper(int delayTime, int repeat, int x = 1, int y = 1);
void runOneStep(int delayTime, int x = 1, int y = 1);

// MARK: setup
void setup() {
  Serial.begin(9600);
  
  // Declare pins as output:
  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  
  Serial.println("Stepper motor test for x and y axis.");
  
  square(500, 5);
  delay(5000);
  diagonal(500, 5);
}

void loop() {}

// MARK: square
void square(int delayTime, int repeat) {
  Serial.println("Stepper motors are going to make a square now.");
  
  // Set the spinning direction counterclockwise && clockwise
  // FORWARD
  digitalWrite(dirPinX, LOW);
  digitalWrite(dirPinY, HIGH);
  Serial.println("Spin the stepper motors in counterclockwise and clockwise direction for a forward movement.");
  
  // Spin the stepper motor 5 revolution Fast:
  runStepper(delayTime, repeat);
  
  // Set the spinning direction counterclockwise && counterclockwise
  // RIGHT
  digitalWrite(dirPinX, LOW);
  digitalWrite(dirPinY, LOW);
  Serial.println("Spin the stepper motors in counterclockwise and counterclockwise direction for a right movement.");
  
  runStepper(delayTime, repeat);
  
  // Set the spinning direction clockwise && counterclockwise
  // BACKWARD
  digitalWrite(dirPinX, HIGH);
  digitalWrite(dirPinY, LOW);
  Serial.println("Spin the stepper motors in clockwise and counterclockwise direction for a backward movement.");
  
  runStepper(delayTime, repeat);
  
  // Set the spinning direction clockwise && clockwise
  // LEFT
  digitalWrite(dirPinX, HIGH);
  digitalWrite(dirPinY, HIGH);
  Serial.println("Spin the stepper motors in clockwise and clockwise direction for a left movement.");
  
  runStepper(delayTime, repeat);
}

// MARK: diagonal
void diagonal(int delayTime, int repeat) {
  Serial.println("Stepper motors are going to make diagonals.");
  
  // Set the spinning direction of LEFT motor to CLOCKWISE
  // FORWARD RIGHT
  Serial.println("Spin the LEFT stepper motor in CLOCKWISE direction for a FORWARD RIGHT movement.");
  digitalWrite(dirPinX, HIGH);
  
  // Spin the stepper motor 5 revolution Fast:
  runStepper(delayTime, repeat, 1, 0);
  
  // Set the spinning direction of LEFT motor to COUNTER CLOCKWISE
  // BACKWARD RIGHT
  Serial.println("Spin the LEFT stepper motor in COUNTER CLOCKWISE direction for a BACKWARD RIGHT movement.");
  digitalWrite(dirPinX, LOW);
  runStepper(delayTime, repeat, 1, 0);
  
  // Set the spinning direction of RIGHT motor to CLOCKWISE
  // FORWARD LEFT
  Serial.println("Spin the LEFT stepper motor in CLOCKWISE direction for a FORWARD LEFT movement.");
  digitalWrite(dirPinY, HIGH);
  runStepper(delayTime, repeat, 0, 1);
  
  // Set the spinning direction of RIGHT motor to COUNTER CLOCKWISE
  // BACKWARD LEFT
  Serial.println("Spin the LEFT stepper motor in COUNTER CLOCKWISE direction for a BACKWARD LEFT movement.");
  digitalWrite(dirPinY, LOW);
  runStepper(delayTime, repeat, 0, 1);
}

// MARK: runStepper
void runStepper(int delayTime, int repeat, int x, int y) {
  for (int i = 0; i < stepsPerRevolution * repeat; i++) {
    runOneStep(delayTime, x, y);
  }
  delay(delayTime);
}

// MARK: runOneStep
void runOneStep(int delayTime, int x, int y) {
  // These six lines result in 1 step:
  if (x) digitalWrite(stepPinX, HIGH);
  if (y) digitalWrite(stepPinY, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepPinX, LOW);
  digitalWrite(stepPinY, LOW);
  delayMicroseconds(delayTime);
}
