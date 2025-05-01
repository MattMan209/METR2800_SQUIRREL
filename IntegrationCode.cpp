// === Warman Sqruirrel Code ===
// Features:
// - 4 DC Motors with 700 CPR encoders (closed-loop control)
// - Mecanum wheel movement (forward, backward, strafe, rotate)
// - 1 NEMA 17 stepper motor using AccelStepper
// - 2 Servos (1 for deployment, 1 for rotation)
// - 3 QTRX-MD-04A sensor arrays: front (edge detect), left/right (edge follow)

#include <AccelStepper.h>
#include <Servo.h>
#include <Encoder.h>
#include <QTRSensors.h>

// === MOTOR & ENCODER DEFINITIONS ===
#define FL_PWM 5  // Front Left
#define FL_DIR 4
#define FR_PWM 6  // Front Right
#define FR_DIR 7
#define BL_PWM 9  // Back Left
#define BL_DIR 8
#define BR_PWM 10 // Back Right
#define BR_DIR 11

Encoder encFL(22, 23);
Encoder encFR(24, 25);
Encoder encBL(26, 27);
Encoder encBR(28, 29);

const float WHEEL_DIAMETER_MM = 60.0;
const int CPR = 700;
const float MM_PER_COUNT = (PI * WHEEL_DIAMETER_MM) / CPR;

// === STEPPER DEFINITIONS ===
#define STEP_PIN 2
#define DIR_PIN 3
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
#define STEP_OUT 32000
#define STEP_IN 0
#define STEP_START 20000

// === SERVOS ===
Servo servo1;
Servo servo2;
#define SERVO1_PIN 12
#define SERVO2_PIN 13

// === QTR SENSORS ===
QTRSensors qtrFront;
uint8_t frontPins[] = {A0, A1, A2, A3};
uint16_t frontValues[4];

QTRSensors qtrLeft;
uint8_t leftPins[] = {A4, A5, A6, A7};
uint16_t leftValues[4];

QTRSensors qtrRight;
uint8_t rightPins[] = {A8, A9, A10, A11};
uint16_t rightValues[4];

// === MOTOR CONTROL ===
void setMotor(int pwmPin, int dirPin, int speed) {
  digitalWrite(dirPin, speed >= 0);
  analogWrite(pwmPin, constrain(abs(speed), 0, 255));
}

void setMecanum(int x, int y, int rot) {
  int fl = y + x + rot;
  int fr = y - x - rot;
  int bl = y - x + rot;
  int br = y + x - rot;

  setMotor(FL_PWM, FL_DIR, fl);
  setMotor(FR_PWM, FR_DIR, fr);
  setMotor(BL_PWM, BL_DIR, bl);
  setMotor(BR_PWM, BR_DIR, br);
}

void stopMotors() {
  setMecanum(0, 0, 0);
}

void resetEncoders() {
  encFL.write(0); encFR.write(0);
  encBL.write(0); encBR.write(0);
}

long averageEncoderCounts() {
  return (abs(encFL.read()) + abs(encFR.read()) + abs(encBL.read()) + abs(encBR.read())) / 4;
}

void driveForwardMM(float mm, int speed) {
  resetEncoders();
  long targetCounts = mm / MM_PER_COUNT;
  while (averageEncoderCounts() < targetCounts) {
    long err = (encFL.read() - encFR.read()) + (encBL.read() - encBR.read());
    int correction = err * 0.1;
    setMecanum(0, speed, -correction);
  }
  stopMotors();
}

void driveBackwardMM(float mm, int speed) {
  driveForwardMM(mm, -speed);
}

// === STEPPER & SERVO ===
void deployArm() {
  stepper.moveTo(STEP_OUT);
  while (stepper.distanceToGo() != 0) stepper.run();
  delay(300);
  servo1.write(90);
  delay(500);
  stepper.moveTo(STEP_IN);
  while (stepper.distanceToGo() != 0) stepper.run();
}

void rotateServo2(int rotations) {
  for (int i = 0; i < rotations * 10; i++) {
    servo2.write(0);
    delay(50);
    servo2.write(180);
    delay(50);
  }
  servo2.write(90);
}

// === QTR SENSOR SETUP ===
void setupSensors() {
  qtrFront.setTypeAnalog();
  qtrFront.setSensorPins(frontPins, 4);
  qtrLeft.setTypeAnalog();
  qtrLeft.setSensorPins(leftPins, 4);
  qtrRight.setTypeAnalog();
  qtrRight.setSensorPins(rightPins, 4);

  delay(500);
  for (uint8_t i = 0; i < 200; i++) {
    qtrFront.calibrate();
    qtrLeft.calibrate();
    qtrRight.calibrate();
    delay(20);
  }
}

// === EDGE FOLLOWING ===
const int ON_TABLE = 900;
const int OFF_TABLE = 100;
const int MIDDLE_VAL = (ON_TABLE + OFF_TABLE) / 2;

void followEdgeLinear(int baseSpeed) {
  while (true) {
    qtrFront.read(frontValues);
    int frontSum = 0;
    for (int i = 0; i < 4; i++) frontSum += frontValues[i];
    frontSum /= 4;

    if (frontSum < (MIDDLE_VAL + 20)) break;
  }

  while (true) {
    qtrLeft.read(leftValues);
    qtrRight.read(rightValues);

    int leftAvg = 0, rightAvg = 0;
    for (int i = 0; i < 4; i++) {
      leftAvg += leftValues[i];
      rightAvg += rightValues[i];
    }
    leftAvg /= 4;
    rightAvg /= 4;

    int edgeError = ((leftAvg + rightAvg) / 2) - MIDDLE_VAL;
    int correction = -edgeError * 0.2;  // proportional gain

    setMecanum(0, baseSpeed, correction);
    delay(10);
  }
  stopMotors();
}

// === SETUP & LOOP ===
void setup() {
  Serial.begin(9600);
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(180);
  servo2.write(90);

  pinMode(FL_PWM, OUTPUT); pinMode(FL_DIR, OUTPUT);
  pinMode(FR_PWM, OUTPUT); pinMode(FR_DIR, OUTPUT);
  pinMode(BL_PWM, OUTPUT); pinMode(BL_DIR, OUTPUT);
  pinMode(BR_PWM, OUTPUT); pinMode(BR_DIR, OUTPUT);

  stepper.setMaxSpeed(2200);
  stepper.setAcceleration(1500);
  stepper.setCurrentPosition(STEP_START);

  setupSensors();
}

void loop() {
  driveForwardMM(400, 100);
  deployArm();
  followEdgeLinear(80);
  driveBackwardMM(700, 100);
  rotateServo2(10);

  while (true) stopMotors();
}
