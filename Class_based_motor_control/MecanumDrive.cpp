#include "MecanumDrive.h"

MecanumDrive::MecanumDrive(int in1, int in2, int pwm, int enca, int encb)
  : _in1(in1), _in2(in2), _pwm(pwm), _enca(enca), _encb(encb), _forwardCount(0), _backwardCount(0) {}

void MecanumDrive::init() {
  pinMode(_in1, OUTPUT);
  pinMode(_in2, OUTPUT);
  pinMode(_pwm, OUTPUT);
  pinMode(_enca, INPUT);
  pinMode(_encb, INPUT);
}

void MecanumDrive::setSpeed(float speed) {
  bool forward = speed >= 0;
  int pwmVal = constrain(abs(speed), 0, 255);
  digitalWrite(_in1, forward ? HIGH : LOW);
  digitalWrite(_in2, forward ? LOW : HIGH);
  analogWrite(_pwm, pwmVal);
}

void MecanumDrive::updateEncoder() {
  if (digitalRead(_encb)) {
    _forwardCount++;
  } else {
    _backwardCount++;
  }
}

long MecanumDrive::getForwardCount() {
  return _forwardCount;
}

long MecanumDrive::getBackwardCount() {
  return _backwardCount;
}
