/* 
 * Code for Arduino
 *
 * Reed switch code that goes through all 64 sensors placed in a 8x8 matrix
 * Using 4 x 74HC4067 multiplexers TODO: Change the name
 * one for two rows since they have 16 input pins
 * the other end of each row provides GND and one takes signal
 * the signal pin of multiplexers are connected to A0 - A3
 *
 * Made by: NIHAL T P
 * GitHub: https://github.com/nihaltp
 * LinkedIn: https://www.linkedin.com/in/nihal-tp
 *
 *
 * Circuit:
 *
 *  V_in
 *   |
 *   R1 (2kΩ)
 *   |
 *  -----> V_out (to ESP8266 A0)
 *   |
 *   R2 (1kΩ)
 *   |
 *  GND
 *
 */

//MUX1 control pins    // TODO: Adjust Pins
#define ROW1 5
#define ROW2 6
#define ROW3 7
#define ROW4 8

#define IN1 A0
#define IN2 A1
#define IN3 A2
#define IN4 A3

// Array to store values from each sensor
int sensorValues[64];

// MARK: setup
void setup() {
  Serial.begin(9600);

  pinMode(ROW1, OUTPUT);
  pinMode(ROW2, OUTPUT);
  pinMode(ROW3, OUTPUT);
  pinMode(ROW4, OUTPUT);

  pinMode(IN1, INPUT);
  pinMode(IN2, INPUT);
  pinMode(IN3, INPUT);
  pinMode(IN4, INPUT);

  digitalWrite(ROW1, LOW);
  digitalWrite(ROW2, LOW);
  digitalWrite(ROW3, LOW);
  digitalWrite(ROW4, LOW);
}

// MARK: loop
void loop() {
  //Loop through and read all 64 values
  for (int i = 0; i < 16; i++) {
    readMux(i);
  }
  printValues();
}

// MARK: readMux
void readMux(int num) {
  // Set MUX to set the 4 pins to read the sensor value
  digitalWrite(ROW1, bitRead(num, 0));
  digitalWrite(ROW2, bitRead(num, 1));
  digitalWrite(ROW3, bitRead(num, 2));
  digitalWrite(ROW4, bitRead(num, 3));

  // Control pins of each sensor is connected
  // so we can read 4 values at the same time
  // mux controls 16 sensors so diffrence is 16

  sensorValues[num] = digitalRead(IN1);
  sensorValues[num + 16] = digitalRead(IN2);
  sensorValues[num + 32] = digitalRead(IN3);
  sensorValues[num + 48] = digitalRead(IN4);
}

// MARK: printValues
void printValues() {
  for (int i = 0; i < 64; i++) {
    if (i % 8 == 0) Serial.println();
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.println();
}
