#include "MecanumDrive.h"
#include <Servo.h>
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

// Servo definitions
const int servo1Pin = 4;  // control pin sweep
const int servo2Pin = 5;  // control pin ramp
const int stopVal = 90;   // stop value for ramp servo
Servo sweep_servo;
Servo ramp_servo;


void setup() {
  Serial.begin(115200);

//Wheel setup
  // Initialise wheels
  frontLeft.init();
  frontRight.init();
  rearLeft.init();
  rearRight.init();
  
  // STBY pins
  digitalWrite(stby, HIGH);
  digitalWrite(stby2, HIGH);
  
  // Attatch interrupts (not included for DR4)
  attachInterrupt(digitalPinToInterrupt(18), FL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(20), FR_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(2), RL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(18), RR_ISR, RISING); // Adjust pin if needed

//Servo setup

  sweep_servo.attach(servo1Pin);
  ramp_servo.attach(servo2Pin);

  // Initial positions
  sweep_servo.write(180);
  ramp_servo.write(90);
  
  Serial.println("Setup complete");

}

void loop() {
  
  Serial.println("Forward");
  move(0,100,0);
  delay(5050);
  sweep_servo.write(95);
  delay(500);
  sweep_servo.write(70);
  delay(800);  // This must make a total of 6350 with previous delay
  move(0,0,0);
  ramp_servo.write(80); //This might need to be 100. Does 90 need to be called after?
  delay(500);    // safety delay
  Serial.println("left");
  move(-90,100,0);
  delay(5000);
  /*
  Serial.println("down left");
  move(180,100,0);
  delay(4000);
  move(0, 0, 0);
  ramp_servo.write(100); // This might nee to be 80
  */
  //Actuate ramp (position unknown)
  


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