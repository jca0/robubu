#pragma once
#include <ESP32Servo.h>
#include "config.h"

Servo myServo;

void initServo() {
  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(90);
}

void setServoAngle(int angle) {
  angle = constrain(angle, 0, 180);
  myServo.write(angle);
}
