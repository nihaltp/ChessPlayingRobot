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
void runStepper(int delayTime, int repeat);
void runOneStep(int delayTime);

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

// MARK: runStepper
void runStepper(int delayTime, int repeat) {
  for (int i = 0; i < stepsPerRevolution * repeat; i++) {
    runOneStep(delayTime);
  }
  delay(delayTime);
}

// MARK: runOneStep
void runOneStep(int delayTime) {
  // These six lines result in 1 step:
  digitalWrite(stepPinX, HIGH);
  digitalWrite(stepPinY, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepPinX, LOW);
  digitalWrite(stepPinY, LOW);
  delayMicroseconds(delayTime);
}
