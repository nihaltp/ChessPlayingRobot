/* 
 * Test Code for checking lcd display using Arduino
 *
 * Made by: NIHAL T P
 * GitHub: https://github.com/nihaltp
 * LinkedIn: https://www.linkedin.com/in/nihal-tp
*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Chess piece characters
byte pawn[8]   = { 0b00100, 0b01110, 0b00100, 0b00100, 0b00100, 0b01110, 0b11111, 0b00000 };
byte rook[8]   = { 0b10101, 0b10101, 0b11111, 0b01110, 0b01110, 0b11111, 0b11111, 0b00000 };
byte knight[8] = { 0b00000, 0b01100, 0b01110, 0b01111, 0b01110, 0b01100, 0b01000, 0b11111 };
byte bishop[8] = { 0b00100, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001, 0b01110, 0b00000 };
byte queen[8]  = { 0b01010, 0b11111, 0b01010, 0b11111, 0b10101, 0b11111, 0b11111, 0b00000 };
byte king[8]   = { 0b00100, 0b01010, 0b00100, 0b01110, 0b10101, 0b11111, 0b11111, 0b00000 };

String pieceNames[] = {"pawn", "rook", "knight", "bishop", "queen", "king"};
int pieceIndex = 0;
#define interval 5000
unsigned long previousMillis = 0;

String message;

void showPieces(void);
void serialDisplay(void);
void displayPiece(int index, uint8_t col, uint8_t row);

// MARK: setup
void setup() {
  // initialize serial communication at 9600
  Serial.begin(9600);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.backlight();// turn on the backlight
  
  // create chess piece characters
  lcd.createChar(0, pawn);
  lcd.createChar(1, rook);
  lcd.createChar(2, knight);
  lcd.createChar(3, bishop);
  lcd.createChar(4, queen);
  lcd.createChar(5, king);
  
  // Remove the cursor
  lcd.noCursor();
}

// MARK: loop
void loop() {
  showPieces();
  serialDisplay();
}

// MARK: showPieces
void showPieces() {
  if (millis() - previousMillis >= interval) {
    pieceIndex = (pieceIndex + 1) % 6;
    
    lcd.setCursor(0, 0);
    String line = "Piece: " + pieceNames[pieceIndex];
    lcd.print(line);
    Serial.println(line);
    
    // Clear remaining characters on the first line up to column 14
    for (int i = line.length(); i < 15; i++) {
      lcd.print(' ');
    }
    displayPiece(pieceIndex, 15, 0);
    
    previousMillis = millis();
  }
}

// MARK: serialDisplay
void serialDisplay() {
  String data;
  
  // when characters arrive over the serial port...
  if (Serial.available()) {
    lcd.setCursor(0, 1); // bottom left
    
    data = Serial.readString();
    data.trim();
    
    // add data to message
    message += data;
    
    // if the length of message is greater than the size of display
    // it removes the first character and displays it until it is the correct size
    while (message.length() > 16) {
      message.remove(0,1);
      lcd.print("                ");  // 16 spaces to clear the row
      lcd.print(message);
      delay(500);
    }
    
    lcd.print(message);
  }
  
  // clear the screen if the serial input is "clear"
  if (data == "clear") {
    lcd.clear();
    message = "";
  }
}

// MARK: displayPiece
void displayPiece(int index, uint8_t col, uint8_t row) {
  // display a specific piece at specific col and row
  lcd.setCursor(col, row);
  lcd.write(byte(index));
}
