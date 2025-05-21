#include "MecanumDrive.h"
#include <Servo.h>


// Instantiate wheel objects
MecanumDrive frontLeft(23, 25, 10, 18, 19);
MecanumDrive frontRight(31, 29, 11, 20, 21);
MecanumDrive rearLeft(47, 45, 13, 2, 3);
MecanumDrive rearRight(51, 53, 12, 18, 19); // Update ENC pins as needed
const int stby = 27;
const int stby2 = 49;

//Sensor Pins NOT IMPLEMENTED YET
const int rFSwitch = 1000;
const int rBSwitch = 1000;
const int lFSwitch = 1000;
const int lBSwitch = 1000;

//Servos NOT IMPLEMENTED YET
const int servoArmPin = 1000;
const int servoSpoolPin = 1000;
Servo ServoArm; 
Servo ServoSpool;


// ISR wrappers
// void FL_ISR() { frontLeft.updateEncoder(); }
// void FR_ISR() { frontRight.updateEncoder(); }
// void RL_ISR() { rearLeft.updateEncoder(); }
// void RR_ISR() { rearRight.updateEncoder(); }

// multiply all motor outputs by the values to ensure motors all spin forwards when set to positive
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

  //Servos Untested In This Code
  ServoArm.attach(servoArmPin); 
	ServoArm.write(0);
	ServoSpool.attach(servoSpoolPin);
	ServoSpool.write(90);


  //attachInterrupt(digitalPinToInterrupt(18), FL_ISR, RISING);
  // attachInterrupt(digitalPinToInterrupt(20), FR_ISR, RISING);
  // attachInterrupt(digitalPinToInterrupt(2), RL_ISR, RISING);
  // attachInterrupt(digitalPinToInterrupt(18), RR_ISR, RISING); // Adjust pin if needed

// STBY pins
  digitalWrite(stby, HIGH);
  digitalWrite(stby2, HIGH);
}

void loop() {
  //start on very back back of box
  brake();
  delay(2000);

  //forward until just on further edge, all time hard coded
  startToEdge();
  delay(2000);
  
  brake();
  delay(1000);
  //Runs left on board along edge
  left(100);
  delay(1500);

  brake();
  while(1){};
}

void startToEdge(){
  // drives from back edge of start box and stops on the exact other edge
  // may need to ajust second delay depending on different factors. 
  delay(2000);
  forward(100);
  delay(3500);
  brake();
}



void brake() {
  // stops all motors
  frontLeft.setSpeed(0);
  frontRight.setSpeed(0);
  rearLeft.setSpeed(0);
  rearRight.setSpeed(0);
}

void forward(float speed) {
  //drives forward at given speed, multiplied by a scaler to straighten
  float flscale = flscaler * 1.0223;
  float frscale = frscaler * 1;
  float rlscale = rlscaler * 1.0223;
  float rrscale = rrscaler * 1;
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
} 

void backward(float speed) {
  //drives backward at given speed, multiplied by a scaler to straighten
  float flscale = -flscaler * 1.0223;
  float frscale = -frscaler * 1;
  float rlscale = -rlscaler * 1.0223;
  float rrscale = -rrscaler * 1;
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
} 



void left(float speed) {
  //Takes a speed and travels left
  //use flscale as the direction pin needed multiplied by as caler to make them run straight
  //accelerate up to speed as am having a problem with slip at the start
  //1.05 0.9 1.2 1 Old numbers that used to work

  float flscale = -flscaler * 1.00;
  float frscale = frscaler * 0.98;
  float rlscale = rlscaler * 1.150;
  float rrscale = -rrscaler * 1.150;
  
  // accelerates motors up to speed
  for (int i = 20; i < speed; i++) {
    frontLeft.setSpeed(flscale * i);
    frontRight.setSpeed(frscale * i);
    rearLeft.setSpeed(rlscale * i);
    rearRight.setSpeed(rrscale * i);
    delay(20); // Change delay higher to slow acceleration
  }
  
  frontLeft.setSpeed(flscale * speed);
  frontRight.setSpeed(frscale * speed);
  rearLeft.setSpeed(rlscale * speed);
  rearRight.setSpeed(rrscale * speed);
} 

void ServoDown() {
  for(int pos = 0; pos <= 110; pos += 1) { // goes from 0 degrees to 110 degrees  
    ServoArm.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       //550ms total round to 560ms with calls
  }
}

void edgingServo(){
  // drive from edge to edge and actuate servo at right time without slowing down
  // Same as above but actuate servo
  // Not Tested, I wrote this very late at night
  delay(2000);
  forward(100);
  delay(2530); //time to start servo
  ServoDown(); // This takes 560ms
  delay(410); //should addd to 3500ms total to stop on edge correctly
  brake();
}
// void edgeAlign() {
//   bool RF = !digitalRead(rFSwitch); //Change ! if it always on or always off
//   bool RB = !digitalRead(rBSwitch);
//   bool LF = !digitalRead(lFSwitch);
//   bool LB = !digitalRead(lBSwitch);

//   if (RF &&)
// }

// Not Implemented

// void leftstraight(float speed) {
//   float flscale = 1;
//   float frscale = 1;
//   float rlscale = 1;
//   float rrscale = 1;
//   frontLeft.setSpeed(flscale * speed);
//   frontRight.setSpeed(frscale * speed);
//   rearLeft.setSpeed(rlscale * speed);
//   rearRight.setSpeed(rrscale * speed);
// } 

// void reverse(float speed) {
  
//   float flscale = -1 * 1;
//   float frscale = -1 * 1;
//   float rlscale = -1 * 1;
//   float rrscale = -1 * 1;
//   frontLeft.setSpeed(flscale * speed);
//   frontRight.setSpeed(frscale * speed);
//   rearLeft.setSpeed(rlscale * speed);
//   rearRight.setSpeed(rrscale * speed);
// } 




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
