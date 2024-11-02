#include <ESP32Servo.h>

#define SERVO_PIN 23
#define RSSI_PIN_LEFT 34
#define RSSI_PIN_RIGHT 35

Servo servoMotor;

int servoPosition = 90; 

void setup() {
  Serial.begin(115200);
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(servoPosition);
}

void loop() {
  int rssiLeft = analogRead(RSSI_PIN_LEFT);
  int rssiRight = analogRead(RSSI_PIN_RIGHT);

  Serial.print("RSSI_L:");
  Serial.print(rssiLeft);
  Serial.print(";RSSI_R:");
  Serial.print(rssiRight);
  Serial.print(";SERVO:");
  Serial.println(servoPosition);

  int rssiDifference = rssiLeft - rssiRight;
  int threshold = 10;


  if (rssiDifference > threshold) {
    servoPosition -= 1;
    if (servoPosition < 0) servoPosition = 0;
    servoMotor.write(servoPosition);
  }

  else if (rssiDifference < -threshold) {
    servoPosition += 1;
    if (servoPosition > 180) servoPosition = 180;
    servoMotor.write(servoPosition);
  }

  delay(20);
}
