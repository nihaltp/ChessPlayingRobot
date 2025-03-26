/* 
 * Version: 2.0
 *
 * Code for Arduino
 *
 * Reed switch code that goes through all 64 sensors placed in a 8x8 matrix
 * Using 4 x 74HC4067 multiplexers TODO: Change the name
 * one for two rows since they have 16 input pins
 * one end of the sensor is connected to GND and the other end gives signal
 * the SIG (signal) pin of multiplexers are shorted and connected to A0
 * the enable pins of multiplexers are connected to digital pins 2, 3, 4, 5
 *
 * Made by: NIHAL T P
 * GitHub: https://github.com/nihaltp
 * LinkedIn: https://www.linkedin.com/in/nihal-tp
 *
 * Circuit:
 *
 *  GND
 *   |
 * Sensor
 *   |
 *  -----> Out (to Arduino)
 *   |
 *  1kΩ (internal pull-up)
 *   |
 *   5V
 *
*/

#include <Arduino.h>

// TODO: Adjust Pins
//MUX control pins
#define ROW1 8   // S0
#define ROW2 9   // S1
#define ROW3 10  // S2
#define ROW4 11  // S3

// MUX enable pins
#define EN1 2    // MUX1
#define EN2 3    // MUX2
#define EN3 4    // MUX3
#define EN4 5    // MUX4

#define IN A0    // Signal

// Array to store values from each sensor
bool sensorValues[64];

void readMux(int num);

// MARK: setup
void setup() {
  Serial.begin(115200);
  
  pinMode(ROW1, OUTPUT);
  pinMode(ROW2, OUTPUT);
  pinMode(ROW3, OUTPUT);
  pinMode(ROW4, OUTPUT);
  
  pinMode(IN, INPUT_PULLUP);
  
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);
  pinMode(EN4, OUTPUT);
  
  // Enable of multiplexer is active low
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(EN4, HIGH);
}

// MARK: loop
void loop() {
  //Loop through and read all 64 values
  Serial.print("<");
  for (int i = 0; i < 64; i++) {
    readMux(i);
    Serial.print(sensorValues[i]);
    if (i < 63) {
      Serial.print(",");
    }
  }
  Serial.println(">");
}

// MARK: readMux
void readMux(int num) {
  // Set MUX to set the 4 pins to read the sensor value
  digitalWrite(ROW1, bitRead(num, 0));
  digitalWrite(ROW2, bitRead(num, 1));
  digitalWrite(ROW3, bitRead(num, 2));
  digitalWrite(ROW4, bitRead(num, 3));
  
  // Enable the corresponding multiplexer based on the sensor number
  digitalWrite(EN1, (num < 16)              ? LOW : HIGH);
  digitalWrite(EN2, (num >= 16 && num < 32) ? LOW : HIGH);
  digitalWrite(EN3, (num >= 32 && num < 48) ? LOW : HIGH);
  digitalWrite(EN4, (num >= 48)             ? LOW : HIGH);
  
  delay(50);
  
  sensorValues[num] = digitalRead(IN);
}
