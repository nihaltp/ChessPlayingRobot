/*
 * Version 1.2.2
 *
 * Sketch to control two stepper motors with
 * A4988/DRV8825 stepper motor driver and
 * Arduino without a library to CORE X Y system
 * 
 * This sketch can move the stepper motors from one position to another
 * 
 * Created by NIHAL T P
 * GitHub: https://github.com/nihaltp
 * LinkedIn: https://www.linkedin.com/in/nihal-tp/
*/

#include <Arduino.h>
#include <string.h>

// Define stepper motor connections and steps per revolution:
#define dirPinX 6   // Direction Pin
#define stepPinX 7  // Step Pin

#define dirPinY 8   // Direction Pin
#define stepPinY 9  // Step Pin

#define stepsPerRevolution 200
#define stepSpeed 500
#define MAX_STEPS 1500  // maximum steps to move in one go

int xPosition = 0;
int yPosition = 0;

#define squareSize 195  // size of each square in steps

const char* files = "hgfedcba";
String currentSquare = "a1";

void handleReset(String data);
void executeMove(String data);
void handleMove(String data);
void extractFileRank(String square, String &file, int &rank);
void moveFromTo(String fromFile, int fromRank, String toFile, int toRank);
bool verifySquare(String square);
void directionPrint(int steps, int xDir, int yDir);
void runStepper(int steps, int xDir, int yDir, int delayTime = stepSpeed);
void runOneStep(int delayTime = stepSpeed);

// MARK: setup
void setup() {
  Serial.begin(115200);
  
  // Declare pins as output:
  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  
  Serial.println("Stepper motors are ready");
  Serial.println("Enter the moves in the format from_square to_square");
  Serial.println("For example, if you want to move from e2 to e4, Send e2e4\n\n");
  
  Serial.println("Other commands:");
  Serial.println("RESET <square> - Reset the current position to the given square (e.g., RESET e2)");
  Serial.println("MOVE <steps>,<xDir>,<yDir> - Move the stepper motors (e.g., MOVE 100,0,0)");
  Serial.println("Direction: 0,0 = FORWARD, \n\t0,1 = LEFT, \n\t1,0 = RIGHT, \n\t1,1 = BACKWARD");
  
  Serial.print("Please send the position of magnet (eg: a1, a8): ");
  // Wait until a valid serial message is received
  while (true) {
    if (Serial.available()) {
      String data = Serial.readString();
      data.trim();
      data.toLowerCase();
      if (verifySquare(data)) {
        currentSquare = data;
        Serial.print("Current Square: ");
        Serial.println(currentSquare);
        break;
      } else {
        Serial.println("Invalid square. Please send a valid position (e.g., b1, h8): ");
      }
    }
  }
}

// MARK: loop
void loop() {
  if (Serial.available()) {
    delay(100);
    
    String data = Serial.readString();
    data.trim();
    data.toLowerCase();
    
    // if data starts with RESET, then reset the current position
    if (data.startsWith("RESET")) {
      // +1 to skip the space after RESET
      data = data.substring(5 + 1);
      handleReset(data);
    }
    // if data starts with MOVE, then move the stepper motors
    else if (data.startsWith("MOVE")) {
      data = data.substring(4 + 1);
      executeMove(data);
    }
    else if (data.length() == 2 || data.length() == 4) {
      handleMove(data);
    }
    else {
      Serial.println("Invalid command: " + data);
    }
  }
}

// MARK: handleReset
void handleReset(String data) {
  if (verifySquare(data)) {
    Serial.println("\nCurrent Square resetted from :" + currentSquare + " to :" + data);
    currentSquare = data;
    return;
  }
  Serial.println("Invalid reset position. Please send the reset position in the format filerank (e.g., e2, e4): ");
  Serial.println("RESET <square> - Reset the current position to the given square (e.g., RESET e2)");
}

// MARK: executeMove
void executeMove(String data) {
  int firstComma = data.indexOf(",");
  int secondComma = data.indexOf(",", firstComma + 1);
  
  if (firstComma == -1 || secondComma == -1) {
    Serial.println("Error: Invalid format. Please use the format: MOVE <steps>,<xDir>,<yDir> - Move the stepper motors (e.g., MOVE 100,0,0)");
    return;
  }
  
  int steps = data.substring(0, firstComma).toInt();
  int xDir = data.substring(firstComma + 1, secondComma).toInt();
  int yDir = data.substring(secondComma + 1).toInt();
  
  // check if the input is less than 0 or greater than MAX_STEPS
  if (steps <= 0 || steps > MAX_STEPS) {
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
  
  directionPrint(steps, xDir, yDir);
  runStepper(steps, xDir, yDir);
}

// MARK: handleMove
void handleMove(String data) {
  if (data.length() == 2) {
    data = currentSquare + data;
  }
  
  String fromSquare = data.substring(0, 2);
  String toSquare = data.substring(2);
  
  if (!verifySquare(fromSquare) || !verifySquare(toSquare)) {
    Serial.println("Invalid move. Please send the move in the format from_square to_square (e.g., e2e4): ");
    return;
  }
  
  String currentFile, fromFile, toFile;
  int currentRank, fromRank, toRank;
  
  extractFileRank(currentSquare, currentFile, currentRank);
  extractFileRank(fromSquare, fromFile, fromRank);
  extractFileRank(toSquare, toFile, toRank);
  
  moveFromTo(currentFile, currentRank, fromFile, fromRank);  // move to the from square
  moveFromTo(fromFile, fromRank, toFile, toRank);            // move to the to square
  
  // update the current square
  currentSquare = toSquare;
}

// MARK: extractFileRank
void extractFileRank(String square, String& file, int& rank) {
  file = square.substring(0, 1);
  String rankStr = square.substring(1, 2);
  rank = rankStr.toInt();
}

// MARK: moveFromTo
void moveFromTo (String fromFile, int fromRank, String toFile, int toRank) {
  // TODO: add diagonal movement
  if (fromFile != toFile) {
    // find if the file we want to move to is on the left or right
    // check the position of the file in the files array
    int toFileIndex = strchr(files, toFile.charAt(0)) - files;
    int fromFileIndex = strchr(files, fromFile.charAt(0)) - files;
    
    // if the index of the file we want to move to is less than the index of the file we are at, then we need to move left, otherwise right
    int xDir = (fromFileIndex < toFileIndex) ? 0 : 1;
    int steps = abs(toFileIndex - fromFileIndex) * squareSize;
    
    directionPrint(steps, xDir, !xDir);
    runStepper(steps, xDir, !xDir);
  }
  if (fromRank != toRank) {
    // find if the rank we want to move to is forward or backward
    int yDir = (fromRank < toRank) ? 0 : 1;
    int steps = abs(toRank - fromRank) * squareSize;
    
    directionPrint(steps, yDir, yDir);
    runStepper(steps, yDir, yDir);
  }
}

// MARK: verifySquare
bool verifySquare(String square) {
  char file = square.charAt(0);
  String rankStr = square.substring(1, 2);
  int rank = rankStr.toInt();
  
  // check if the square is valid
  char* position = strchr(files, file);
  if (position == NULL) {
    return false;
  }
  if (rank < 1 || rank > 8) {
    return false;
  }
  return true;
}

// MARK: directionPrint
void directionPrint(int steps, int xDir, int yDir) {
  // print the direction of movement according to xDir and yDir
  Serial.print("Moving ");
  if (xDir == 0) {
    if (yDir == 0) {
      xPosition += steps;  // add steps to xPosition
      Serial.print("FORWARD ");
    } else {
      yPosition += steps;  // add steps to yPosition
      Serial.print("RIGHT ");
    }
  } else {
    if (yDir == 0) {
      yPosition -= steps;  // remove steps from yPosition
      Serial.print("LEFT ");
    } else {
      xPosition -= steps;  // remove steps from xPosition
      Serial.print("BACKWARD ");
    }
  }
  Serial.println(steps);
  Serial.println("Position (X,Y): " + String(xPosition) + ", " + String(yPosition));
}

// MARK: runStepper
void runStepper(int steps, int xDir, int yDir, int delayTime) {
  // Sets the spinning direction:
  // LOW - runs clockwise, HIGH - runs counterclockwise
  digitalWrite(dirPinX, (xDir == 0) ? LOW : HIGH);
  digitalWrite(dirPinY, (yDir == 0) ? HIGH : LOW);
  
  for (int i = 0; i < steps; i++) {
    runOneStep(delayTime);
  }
  delay(delayTime);
}

// MARK: runOneStep
void runOneStep(int delayTime) {
  // rotates both the stepper motors at the same time
  // These six lines result in 1 step:
  digitalWrite(stepPinX, HIGH);
  digitalWrite(stepPinY, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepPinX, LOW);
  digitalWrite(stepPinY, LOW);
  delayMicroseconds(delayTime);
}
