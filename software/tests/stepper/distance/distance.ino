/* 
 * Version: 3.0
 *
 * Example sketch to control two stepper motors with
 * A4988/DRV8825 stepper motor driver and
 * Arduino without a library, to control CORE X Y system
 * 
 * This sketch can be used to find steps required
 * for moving a certain distance in X and Y directions
 * 
 * Created by NIHAL T P
 * GitHub: https://github.com/nihaltp
 * LinkedIn: https://www.linkedin.com/in/nihal-tp/
*/

#include <Arduino.h>

// Define stepper motor connections and steps per revolution:
#define dirPinX 6   // Direction Pin
#define stepPinX 7  // Step Pin

#define dirPinY 8   // Direction Pin
#define stepPinY 9  // Step Pin

#define stepsPerRevolution 200

// make sure the system doesnt go out of the border
#define maxStepsX 1500
#define maxStepsY 1500
#define maxSteps max(maxStepsX, maxStepsY)

int xDistance = 0;
int yDistance = 0;

void bound(int steps, int xDir, int yDir);
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
  
  Serial.println("Stepper motors ready for testing");
  Serial.println("\n*Make sure the stepper motors are at their initial position*");
  Serial.println("\nEnter the number of steps to move with the direction of the stepper motors");
  Serial.println("Enter the data in the format: step,x,y");
  Serial.println("\nDirection: 0,0 = FORWARD, \n\t0,1 = RIGHT, \n\t1,0 = LEFT, \n\t1,1 = BACKWARD");
  Serial.println("For example, if you want to move the stepper motor 100 steps in the forward direction, enter: 100,0,0\n");
  Serial.println("Make sure the system doesn't go out of the border: " + String(maxStepsX) + ", " + String(maxStepsY));
}

// MARK: loop
void loop() {
  if (Serial.available()) {
    delay(100);
    
    String data = Serial.readString();
    data.trim();
  
    int firstComma = data.indexOf(",");
    int secondComma = data.indexOf(",", firstComma + 1);
    
    int steps = data.substring(0, firstComma).toInt();
    int xDir = data.substring(firstComma + 1, secondComma).toInt();
    int yDir = data.substring(secondComma + 1).toInt();
    
    if (steps < 0 || steps > maxSteps) {
      Serial.println("Invalid steps");
      return;
    }
    if (xDir != 0 && xDir != 1) {
      Serial.println("Invalid x direction");
      return;
    }
    if (yDir != 0 && yDir != 1) {
      Serial.println("Invalid y direction");
      return;
    }
    
    Serial.println("Moving the stepper motors " + String(steps) + " steps in the direction: " + String(xDir) + "," + String(yDir));
    
    bound(steps, xDir, yDir);
  }
}

// MARK: bound
void bound(int steps, int xDirection, int yDirection) {
  // Temporary variables to store the new position
  int xNew = xDistance;
  int yNew = yDistance;
  
  // find the new position and make sure it doesn't go out 
  // of the border. if it goes out of the border, update 
  // the steps accordingly to make sure it doesn't go out of the border
  if (xDirection == 0) {
    if (yDirection == 0) {
      xNew += steps;
      if (xNew > maxStepsX) {
        steps = maxStepsX - xNew;
        xNew = maxStepsX;
      }
    } else {
      yNew += steps;
      if (yNew > maxStepsY) {
        steps = maxStepsY - yNew;
        yNew = maxStepsY;
      }
    }
  } else {
    if (yDirection == 0) {
      yNew -= steps;
      if (yNew < 0) {
        steps = -yNew;
        yNew = 0;
      }
    } else {
      xNew -= steps;
      if (xNew < 0) {
        steps = -xNew;
        xNew = 0;
      }
    }
  }
  
  // update the position
  xDistance = xNew;
  yDistance = yNew;
  
  // print the direction of movement according to xDir and yDir
  String directionString;
  if (xDirection == 0) {
    directionString = (yDirection == 0) ? "FORWARD" : "RIGHT";
  } else {
    directionString = (yDirection == 0) ? "LEFT" : "BACKWARD";
  }
  
  Serial.println(directionString + " " + String(steps));
  Serial.println("Position (X,Y): " + String(xDistance) + ", " + String(yDistance));
  runStepper(500, steps, xDirection, yDirection);
}

// MARK: runStepper
void runStepper(int delayTime, int steps, int xDir, int yDir) {
  // Sets the spinning direction:
  digitalWrite(dirPinX, (xDir == 0) ? LOW : HIGH);
  digitalWrite(dirPinY, (yDir == 0) ? HIGH : LOW);
  
  for (int i = 0; i < steps; i++) {
    runOneStep(delayTime);
  }
  delay(delayTime);
}

// MARK: runOneStep
void runOneStep(int delayTime = 500) {
  // rotates both the stepper motors at the same time
  // These six lines result in 1 step:
  digitalWrite(stepPinX, HIGH);
  digitalWrite(stepPinY, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepPinX, LOW);
  digitalWrite(stepPinY, LOW);
  delayMicroseconds(delayTime);
}
