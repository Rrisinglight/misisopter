#include <ESP32Servo.h>

#define SERVO_PIN 23
#define RSSI_PIN_LEFT 34
#define RSSI_PIN_RIGHT 35

#define MOVE_LEFT_PIN 2
#define MOVE_RIGHT_PIN 4
#define AUTO_SCAN_PIN 5

Servo servoMotor;

int servoPosition = 90;
bool autoScanning = false;

void setup() {
  Serial.begin(115200);
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(servoPosition);

  pinMode(MOVE_LEFT_PIN, INPUT_PULLDOWN);
  pinMode(MOVE_RIGHT_PIN, INPUT_PULLDOWN);
  pinMode(AUTO_SCAN_PIN, INPUT_PULLDOWN);
}

void loop() {
  bool moveLeft = digitalRead(MOVE_LEFT_PIN);
  bool moveRight = digitalRead(MOVE_RIGHT_PIN);
  bool startAutoScan = digitalRead(AUTO_SCAN_PIN);

  if (startAutoScan && !autoScanning) {
    autoScanning = true;
    performAutoScan();
    autoScanning = false;
  }

  if (moveLeft && !autoScanning) {
    servoPosition -= 1;
    if (servoPosition < 0) servoPosition = 0;
    servoMotor.write(servoPosition);
  }

  if (moveRight && !autoScanning) {
    servoPosition += 1;
    if (servoPosition > 180) servoPosition = 180;
    servoMotor.write(servoPosition);
  }

  int rssiLeft = analogRead(RSSI_PIN_LEFT);
  int rssiRight = analogRead(RSSI_PIN_RIGHT);

  Serial.print("RSSI_L:");
  Serial.print(rssiLeft);
  Serial.print(";RSSI_R:");
  Serial.print(rssiRight);
  Serial.print(";SERVO:");
  Serial.println(servoPosition);

  delay(20);
}

void performAutoScan() {
  int maxRSSI = -1;
  int maxPosition = servoPosition;

  for (int pos = 0; pos <= 180; pos += 1) {
    servoMotor.write(pos);
    delay(20);

    int rssiLeft = analogRead(RSSI_PIN_LEFT);
    int rssiRight = analogRead(RSSI_PIN_RIGHT);
    int rssiSum = rssiLeft + rssiRight;

    if (rssiSum > maxRSSI) {
      maxRSSI = rssiSum;
      maxPosition = pos;
    }

    Serial.print("Scanning Pos:");
    Serial.print(pos);
    Serial.print(";RSSI_L:");
    Serial.print(rssiLeft);
    Serial.print(";RSSI_R:");
    Serial.println(rssiRight);
  }

  servoPosition = maxPosition;
  servoMotor.write(servoPosition);
  Serial.print("Max RSSI Position:");
  Serial.println(servoPosition);
}
