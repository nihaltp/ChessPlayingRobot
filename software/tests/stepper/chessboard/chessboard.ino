/*
 * Version 1.1.0
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

int xPosition = 0;
int yPosition = 0;

#define squareSize 75  // size of each square in steps

const char* files = "abcdefgh";
String currentSquare = "a1";

void extractFileRank(String square, String& file, int& rank);
void moveFromTo(String fromFile, int fromRank, String toFile, int toRank);
bool verifySquare(String square);
void directionPrint(int steps, int xDir, int yDir);
void runStepper(int steps, int xDir, int yDir, int delayTime = stepSpeed);
void runOneStep(int delayTime);

// MARK: setup
void setup() {
  Serial.begin(9600);
  
  // Declare pins as output:
  pinMode(stepPinX, OUTPUT);
  pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT);
  pinMode(dirPinY, OUTPUT);
  
  Serial.println("Stepper motors are ready");
  Serial.println("Enter the moves in the format from_square to_square");
  Serial.println("For example, if you want to move from e2 to e4, Send e2e4\n\n");
  
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
      data = data.substring(4 + 1);
      handleReset(data);
    } else {
      handleMove(data);
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
  while (true)
  {
    if (Serial.available()) {
      delay(100);
      
      String data = Serial.readString();
      data.trim();
      data.toLowerCase();
      if (verifySquare(data)) {
        Serial.println("\nCurrent Square resetted from :" + currentSquare + " to :" + data);
        currentSquare = data;
        break;
      } else {
        Serial.println("Invalid reset position. Please send the reset position in the format filerank (e.g., a1, g8): ");
      }
    }
  }
}

// MARK: handleMove
void handleMove(String data) {
  if (data.length() != 4) {
    Serial.println("Invalid move. Please send the move in the format from_square to_square (e.g., e2e4, a6a4): ");
    return;
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
  if (fromFile != toFile) {
    // find if the file we want to move to is on the left or right
    // check the position of the file in the files array
    int toFileIndex = strchr(files, toFile.charAt(0)) - files;
    int fromFileIndex = strchr(files, fromFile.charAt(0)) - files;
    
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
  digitalWrite(dirPinX, (xDir == 0) ? LOW : HIGH);
  digitalWrite(dirPinY, (yDir == 0) ? HIGH : LOW);
  
  for (int i = 0; i < steps; i++) {
    runOneStep(delayTime);
  }
  delay(delayTime);
}

// MARK: runOneStep
void runOneStep(int delayTime = stepSpeed) {
  // rotates both the stepper motors at the same time
  // These six lines result in 1 step:
  digitalWrite(stepPinX, HIGH);
  digitalWrite(stepPinY, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(stepPinX, LOW);
  digitalWrite(stepPinY, LOW);
  delayMicroseconds(delayTime);
}
