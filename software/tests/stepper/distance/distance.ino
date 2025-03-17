/* 
 * Example sketch to control two stepper motors with
 * A4988/DRV8825 stepper motor driver and
 * Arduino without a library to move in x-y axis
 * 
 * This sketch can be used to find steps required for moving a certain distance
 * 
 * Created by NIHAL T P
 * GitHub: https://github.com/nihaltp
 * LinkedIn: https://www.linkedin.com/in/nihal-tp/
*/

#include <Arduino.h>

// Define stepper motor connections and steps per revolution:
#define dirPinX  2  // Direction Pin
#define stepPinX 3  // Step Pin

#define dirPinY  4  // Direction Pin
#define stepPinY 5  // Step Pin

#define stepsPerRevolution 200

void directionPrint(int xDir, int yDir);
void runStepper(int delayTime, int steps, int xDir, int yDir);
void runOneStep(int delayTime);

// MARK: setup
void setup() {
  Serial.begin(9600);
  
  // Declare pins as output:
  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  
  Serial.println("Stepper motor ready for testing");
  Serial.println();
  Serial.println();
  Serial.println("Enter the number of steps to move with the direction of the stepper motors");
  Serial.println("Enter the data in the format: step,x,y");
  Serial.println();
  Serial.println("Direction: 0,0 = FORWARD, 0,1 = RIGHT, 1,0 = LEFT, 1,1 = BACKWARD");
  Serial.println("For example, if you want to move the stepper motor 100 steps in the forward direction, enter: 100,0,0");
  Serial.println();
}

// MARK: loop
void loop() {
  if (Serial.available()) {
    delay(100);
    
    String data = Serial.readString();
    data.trim();
    
    int steps = data.substring(0, data.indexOf(",")).toInt();
    int xDir = data.substring(data.indexOf(",") + 1, data.indexOf(",", data.indexOf(",") + 1)).toInt();
    int yDir = data.substring(data.indexOf(",", data.indexOf(",") + 1) + 1).toInt();
        
    Serial.println("Moving the stepper motors " + String(steps) + " steps in the direction: " + String(xDir) + "," + String(yDir));
    
    directionPrint(xDir, yDir);
    runStepper(500, steps, xDir, yDir);
  }
}

// MARK: directionPrint
void directionPrint(int xDir, int yDir)
{
  // print the direction of movement according to xDir and yDir
  if (xDir == 0)
  {
    if (yDir == 0)
      Serial.println("FORWARD");
    else
      Serial.println("RIGHT");
  }
  else
  {
    if (yDir == 0)
      Serial.println("LEFT");
    else
      Serial.println("BACKWARD");
  }
}

// MARK: runStepper
void runStepper(int delayTime, int steps, int xDir, int yDir) {
  digitalWrite(dirPinX, (xDir == 0) ? LOW : HIGH);
  digitalWrite(dirPinY, (yDir == 0) ? HIGH : LOW);
  for (int i = 0; i < steps; i++) {
    runOneStep(delayTime);
  }
  delay(delayTime);
}

// MARK: runOneStep
void runOneStep(int delayTime = 500) {
  // These six lines result in 1 step:
  digitalWrite(stepPinX, HIGH);
  digitalWrite(stepPinY, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepPinX, LOW);
  digitalWrite(stepPinY, LOW);
  delayMicroseconds(delayTime);
}
