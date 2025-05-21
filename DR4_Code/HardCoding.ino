#include "MecanumDrive.h"

// Instantiate wheel objects
//////////DOOOO NOTTTT CHAAANGE////////////
MecanumDrive frontLeft(23, 25, 10, 18, 19);
MecanumDrive frontRight(31, 29, 11, 20, 21);
MecanumDrive rearLeft(47, 45, 13, 2, 3);
MecanumDrive rearRight(51, 53, 12, 18, 19); // Update ENC pins as needed
const int stby = 27;
const int stby2 = 49;


// ISR wrappers
// void FL_ISR() { frontLeft.updateEncoder(); }
// void FR_ISR() { frontRight.updateEncoder(); }
// void RL_ISR() { rearLeft.updateEncoder(); }
// void RR_ISR() { rearRight.updateEncoder(); }

const int flscaler = 1;
const int frscaler = -1;
const int rlscaler = -1;
const int rrscaler = 1;


void setup() {
  Serial.begin(115200);
  frontLeft.init();
  frontRight.init();
  rearLeft.init();
  rearRight.init();

  //attachInterrupt(digitalPinToInterrupt(18), FL_ISR, RISING);
  // attachInterrupt(digitalPinToInterrupt(20), FR_ISR, RISING);
  // attachInterrupt(digitalPinToInterrupt(2), RL_ISR, RISING);
  // attachInterrupt(digitalPinToInterrupt(18), RR_ISR, RISING); // Adjust pin if needed

// STBY pins
  digitalWrite(stby, HIGH);
  digitalWrite(stby2, HIGH);
}

void loop() {
  edging();
  delay(2000);
  left(100);

  brake();
  while(1){};
}

void edging(){
  delay(2000);
  forward(100);
  delay(3500);
  brake();
}

void brake() {
  frontLeft.setSpeed(0);
  frontRight.setSpeed(0);
  rearLeft.setSpeed(0);
  rearRight.setSpeed(0);
}

void forward(float speed) {
  float flscale = flscaler * 1.0223;
  float frscale = frscaler * 1;
  float rlscale = rlscaler * 1.0223;
  float rrscale = rrscaler * 1;
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
} 
void reverse(float speed) {
  
  float flscale = -1 * 1;
  float frscale = -1 * 1;
  float rlscale = -1 * 1;
  float rrscale = -1 * 1;
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
} 

void left(float speed) {
  //1.05 0.9 1.2 1
  float flscale = -flscaler * 1.00;
  float frscale = frscaler * 1.00;
  float rlscale = rlscaler * 1.12;
  float rrscale = -rrscaler * 1.09;
  for (int i = 0; i < speed; i++) {
    frontLeft.setSpeed(flscale * i);
    frontRight.setSpeed(frscale * i);
    rearLeft.setSpeed(rlscale * i);
    rearRight.setSpeed(rrscale * i);
    delay(20);
  }
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
  delay(3000);
} 

void leftstraight(float speed) {
  float flscale = 1;
  float frscale = 1;
  float rlscale = 1;
  float rrscale = 1;
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
} 






void move(float angle_deg, float max_wheel_speed, float rotation_speed) {
  float fl = 0;
  float fr = 0;
  float rl = 0;
  float rr = 0;
  float scale = 0;
  if (angle_deg  = -90){
    fl = -max_wheel_speed;
    fr = max_wheel_speed;
    rl = max_wheel_speed;
    rr = -max_wheel_speed;
  }
  else if (angle_deg = 90){
    fl = max_wheel_speed;
    fr = -max_wheel_speed;
    rl = -max_wheel_speed;
    rr = max_wheel_speed;
  }
  else{
    float angle_rad = radians(angle_deg);
    float vx = cos(angle_rad);
    float vy = sin(angle_rad);

    fl = vy + vx - rotation_speed;
    fr = vy - vx + rotation_speed;
    rl = vy - vx - rotation_speed;
    rr = vy + vx + rotation_speed;

    float max_raw = max(max(abs(fl), abs(fr)), max(abs(rl), abs(rr)));
    scale = (max_raw > 0) ? (max_wheel_speed / max_raw) : 0;
  }
  
  frontLeft.setSpeed(fl * scale);
  frontRight.setSpeed(fr * scale);
  rearLeft.setSpeed(rl * scale);
  rearRight.setSpeed(rr * scale);
}
