#include "MecanumDrive.h"

// Instantiate wheel objects
MecanumDrive frontLeft(23, 25, 10, 18, 19);
MecanumDrive frontRight(31, 29, 11, 20, 21);
MecanumDrive rearLeft(47, 45, 13, 2, 3);
MecanumDrive rearRight(51, 53, 12, 18, 19); // Update ENC pins as needed
const int stby = 27;
const int stby2 = 49;

// ISR wrappers
void FL_ISR() { frontLeft.updateEncoder(); }
void FR_ISR() { frontRight.updateEncoder(); }
void RL_ISR() { rearLeft.updateEncoder(); }
void RR_ISR() { rearRight.updateEncoder(); }

void setup() {
  Serial.begin(115200);
  frontLeft.init();
  frontRight.init();
  rearLeft.init();
  rearRight.init();

  attachInterrupt(digitalPinToInterrupt(18), FL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(20), FR_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(2), RL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(18), RR_ISR, RISING); // Adjust pin if needed

// STBY pins
  digitalWrite(stby, HIGH);
  digitalWrite(stby2, HIGH);
}

void loop() {
  move(0, 75,0);
  delay(2000);
  move(0,0,0);
  delay(1000);
  move(-90,75,0);
  delay(2000);
  move(0,0,0);
  delay(1000);
  move(90,75,0);
  delay(2000);
  move(0,0,0);
  delay(1000);
  move(180,75,0);
  delay(2000);
  move(0,0,0);

  while(1){};
}

void move(float angle_deg, float max_wheel_speed, float rotation_speed) {

  float angle_rad = radians(angle_deg);
  float vx = cos(angle_rad);
  float vy = sin(angle_rad);

  // Unscaled wheel speeds with rotation
  float fl = vy + vx - rotation_speed;
  float fr = vy - vx + rotation_speed;
  float rl = vy - vx - rotation_speed;
  float rr = vy + vx + rotation_speed;

  float max_raw = max(max(abs(fl), abs(fr)), max(abs(rl), abs(rr)));
  float scale = (max_raw > 0) ? (max_wheel_speed / max_raw) : 0;

  
  frontLeft.setSpeed(fl * scale);
  frontRight.setSpeed(fr * scale);
  rearLeft.setSpeed(rl * scale);
  rearRight.setSpeed(rr * scale);
}
