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

//Sensor Pin
const int FRPin = A13; //Front Right
const int BRPin = A12; //Back Right
const int BLPin = A11; //Back Left
const int FLPin = A10; //Front Left


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
  sweep_servo.attach(servo2Pin);
  ramp_servo.attach(servo1Pin);

  //Limit Switch Setup use internal pullup
  pinMode(FRPin, INPUT_PULLUP);
  pinMode(BRPin, INPUT_PULLUP);
  pinMode(BLPin, INPUT_PULLUP);
  pinMode(FLPin, INPUT_PULLUP);

  // Initial positions
  sweep_servo.write(90);//Should be 90
  ramp_servo.write(90);
  
  Serial.println("Setup complete");

}

void loop() {
  //Start, write servos to their positions
  //Drop ramp one slightly
  delay(2000);
  sweep_servo.write(170);
  delay(200);
  ramp_servo.write(60);
  delay(200);
  ramp_servo.write(90);

  //Do the forward movement
  Serial.println("Forward");
  move(0,100,0); //Forward for 5050ms
  delay(5050);
  sweep_servo.write(70);
  delay(100);

  //Wait until the robot hits the edge
  waitForEdge();
  move(0,0,0);
  delay(200);
  move(180,60,0);//move back off edge slightly 150ms
  delay(150); //this worked at 300 maybe go back
  move(0,0,0);

  //Driving down the left edge
  delay(200);
  driveLeftEdge(1910, 10);
  move(0,0,0);
  delay(200);
  move2(-100,0,0);
  delay(3500);
  move(0,0,0);
  delay(200);

  //Lift up ramp to drop balls
  ramp_servo.write(160);
  delay(1500);
  //ramp_servo.write(80);
  sweep_servo.write(90);
  delay(100);

  //Shake to make sure all the balls come out.
  ramp_servo.write(140);
  delay(600);
  ramp_servo.write(80);
  delay(100);
  move2(20,0,0);
  ramp_servo.write(130);
  delay(300);
  move2(-20,0,0);
  ramp_servo.write(80);
  delay(100);
  move2(20,0,0);
  ramp_servo.write(130);
  delay(300);
  move2(-20,0,0);
  ramp_servo.write(80);
  delay(100);
  move2(20,0,0);
  ramp_servo.write(130);
  delay(300);
  move2(-20,0,0);
  ramp_servo.write(80);
  delay(100);
  move2(20,0,0);
  ramp_servo.write(130);
  delay(300);
  move(0,0,0);
  //ramp_servo.write(60);
  //delay(800);
  ramp_servo.write(90);

  //drive off deposit zone
  move(0,100,0);
  delay(300);
  move(0,0,0);

  // Lift Sweep to demonstrate finish
  sweep_servo.write(180);
  move(0,0,0);

  //end
  while(1){};







  delay(10000000000000000);

  delay(10000000000000);

  
  //Testing Edge Tracking
  //movementTest();
  //ramp_servo.write(90);
  delay(2000);
  waitForEdge();
  move(0,0,0);
  delay(2000);
  
  driveLeftEdge(250, 100);
  move(0,0,0);
  delay(10000000000000000);


  // while(1){};
  // delay(3000);
  // ramp_servo.write(110);
  // delay(1000);
  // ramp_servo.write(90);


  sweep_servo.write(170);
  delay(1000);
  sweep_servo.write(70);


  delay(10000000000000000);
  delay(5000);
  rotateInPlace(100);
  delay(5000);
  driveLeftEdge(500, 100);
  delay(10000000000000000);

  //Run track edge 500 times   
  for (int i = 0; i < 500; i++){
    edgeTrackLeft();
    Serial.print(i);
  }

  move(0,0,0);
  while(1){};


  move(0,0,0);
  delay(2000);
  sweep_servo.write(170);
  delay(200);
  Serial.println("Forward");
  move(0,100,0); //Forward for 5050ms
  delay(5050);
  move(0,0,0);
  delay(10000000000000);
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
  //move function to controll all angles, speeds and roations
  //Angle 0 is forward, 270 is left
  //Positive rotation is anticlockwise

  float angle_rad = radians(angle_deg);
  float vx = cos(angle_rad);
  float vy = sin(angle_rad);

  // Unscaled wheel speeds with rotation
  // Anti clockwise is positive rotation speed
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


void sensorRead(){
  //Prints to serial limit switch states 1000 times
  //Used for testing and debugging
  for (int i = 0; i < 1000; i++){
    int FRState = digitalRead(FRPin); //FR
    int BRState = digitalRead(BRPin); //BR
    int BLState = digitalRead(BLPin); //BL
    int FLState = digitalRead(FLPin); //FL

    Serial.print(FRState);
    Serial.print(BRState);
    Serial.print(FLState);
    Serial.print(BLState);
    Serial.println();
    delay(10);

  }
  Serial.println("Sensor Finished");

}

void waitForEdge() {
  while (stopOnEdge() == 1) {
    //delay(80); // Small delay for stability
  }
  //Serial.println("Edge detected! Stopped.");
}

int stopOnEdge() {
  //Goes forward until any pins read Low(off edge)
  int FLState = !digitalRead(FLPin);
  int BLState = !digitalRead(BLPin);
  int FRState = !digitalRead(FRPin);
  int BRState = !digitalRead(BRPin);

  if (FLState && BLState && FRState && BRState) {
    move(0,0,0);
    delay(80);
    return 0;
    }
  else {
    move(0,80,0);
    return 1;
    }
}

// void edgeTrack() {
//   //Track the left Edge using limit switches

//   //Read sensor states
//   int FL = digitalRead(FLPin); // Front Left
//   int BL = digitalRead(BLPin); // Back Left
//   int FR = digitalRead(FRPin); // Front Right
//   int BR = digitalRead(BRPin); // Back Right

//   // Pair logic
//   bool leftEdge   = (FL != BL); // Edge between left switches
//   bool rightEdge  = (FR != BR); // Edge between right switches
//   bool leftOn     = (FL == HIGH && BL == HIGH);
//   bool rightOn    = (FR == HIGH && BR == HIGH);
//   bool leftOff    = (FL == LOW && BL == LOW);
//   bool rightOff   = (FR == LOW && BR == LOW);

//   // 1. Fully off the table: All off
//   if (leftOff && rightOff) {
//     Serial.println("Off the table: Moving right to regain edge, then stopping");
//     move(270, 60, 0); // Move right (backwards on x), slow, to get back on edge
//     delay(100);
//     return;
//   }

//   // 2. Perfect alignment: Both edge pairs have one pressed and one not
//   if (leftEdge && rightEdge) {
//     Serial.println("Perfect edge alignment: Moving left");
//     move(270, 100, 0);
//     delay(100);
//   }
//   // 3. Too far onto table on both sides (all pressed)
//   else if (leftOn && rightOn) {
//     Serial.println("Too far onto table: Moving left and rotating left");
//     move(270, 80, -0.6);
//     delay(100);
//   }
//   // 4. Too far onto table on left only
//   else if (leftOn) {
//     Serial.println("Left side too far on table: Moving left and rotating left");
//     move(270, 80, -0.4);
//     delay(100);
//   }
//   // 5. Too far onto table on right only
//   else if (rightOn) {
//     Serial.println("Right side too far on table: Moving left and rotating right");
//     move(270, 80, 0.4);
//     delay(100);
//   }
//   // 6. Too far off table on left only
//   else if (leftOff) {
//     Serial.println("Left side off edge: Moving left and rotating right");
//     move(270, 80, 0.6);
//     delay(100);
//   }
//   // 7. Too far off table on right only
//   else if (rightOff) {
//     Serial.println("Right side off edge: Moving left and rotating left");
//     move(270, 80, -0.6);
//     delay(100);
//   }
//   // 8. Both pairs straddling edge but not perfect (fallback correction)
//   else {
//     Serial.println("Unknown or transition state: Moving left slowly");
//     move(270, 60, 0);
//     delay(100);
//   }
//   delay(100);
// }

const int slowRotationSpeed = 7; //was 5
const int fastRotationSpeed = 12; //was 10

const int ySpeed = 80;
const int xSpeedSlow = 5;
const int xSpeedFast = 15;

void edgeTrackLeft() {
  //Track the left Edge using limit switches

  //Read sensor states
  int FL = !digitalRead(FLPin); // Front Left
  int RL = !digitalRead(BLPin); // Back Left
  int FR = !digitalRead(FRPin); // Front Right
  int RR = !digitalRead(BRPin); // Back Right

  if (!FL && !FR && !RL && !RR) { 
    //All on edge - Move 315degrees | No Rotation
    //------------
    //  |x      x|
    //  |x      x|
    Serial.println("All on board - Move 315degrees | No Rotation");
    //move(280,angleSpeed,0);}
    move2(xSpeedFast,ySpeed,0);}

  else if (FL && !FR && !RL && !RR) { 
    //Front Left Off - Move 247.5degrees | Anti Clockwise Slow
    //  |o   ------
    //------    x|
    //  |x      x|
    Serial.println("Front Left Off - Move 247.5degrees | Anti Clockwise Slow");
    //move(263,angleSpeed,-slowRotationSpeed);}
    move2(xSpeedSlow,ySpeed,slowRotationSpeed);}

  else if (FL && FR && !RL && !RR) { 
    //Perfectly Aligned - Move 270degrees | No Rotation
    //  |o      o|
    //------------
    //  |x      x|
    Serial.println("Perfectly Aligned - Move 270degrees | No Rotation");
    move2(0,ySpeed,0);}

  else if (!FL && FR && !RL && !RR) { 
    //Front Right Off - Move 292degrees | Clockwise Slow
    //-------   o|
    //  |o   --------
    //  |x      x|
    Serial.println("Front Right Off - Move 292degrees | Clockwise Slow");
    //move(272,angleSpeed,slowRotationSpeed);}
    move2(-xSpeedSlow,ySpeed,-slowRotationSpeed);}

  else if (FL && !FR && RL && !RR) { 
    //All left off - Move 225 degrees | Anti Clockwise Fast
    //  |o    -   x|
    //  |o   -    x|
    Serial.println("All left off - Move 225 degrees | Anti Clockwise Fast");
    //move(265,angleSpeed,-fastRotationSpeed);}
    move2(xSpeedFast,ySpeed,fastRotationSpeed);}

  else if (!FL && FR && !RL && RR) { 
    //All right off - Move 315 degrees | Clockwise Fast
    //  |x   -    o|
    //  |x    -   o|
    Serial.println("All right off - Move 315 degrees | Clockwise Fast");
    //move(280,angleSpeed,fastRotationSpeed);}
    move2(-xSpeedSlow,ySpeed,-fastRotationSpeed);}

  else if (FL && FR && RL && RR) { 
    //All Off - Move 225 degrees | No Rotation
    //  |0        o|
    //  |0        o|
    //----------------
    Serial.println("All Off - Move 225 degrees | No Rotation");
    //move(-,angleSpeed,0);}
    move2(-xSpeedFast,ySpeed,0);}
  
  else {
    //Unknown Orientation - Backwards | No Rotation
    Serial.println("Unknown Orientation - Backwards | No Rotation");
    //move(180,angleSpeed,0);}
    move2(xSpeedSlow,ySpeed,0);}
}


void driveLeftEdge(int cycles, int cycleDelay) {
  for (int i = 0; i < cycles; i++){
    edgeTrackLeft();
    delay(cycleDelay);
    Serial.print(i);
    Serial.println();
  }
  move(0,0,0);
}

void rotateInPlace(float speed) {
  // speed: positive for counterclockwise (left), negative for clockwise (right)
  // speed range: -max_wheel_speed to +max_wheel_speed

  frontLeft.setSpeed(speed);
  frontRight.setSpeed(-speed);
  rearLeft.setSpeed(speed);
  rearRight.setSpeed(-speed);
}

void movementTest(){
  delay(2000);
  Serial.println("270");
  move(0,100,0);

  delay(2000);
  Serial.println("315");
  move(90,100,0);

  delay(2000);
  Serial.println("248");
  move(180,100,0.3);

  delay(2000);
  Serial.println("270");
  move(270,100,0);
  
  delay(2000);
  Serial.println("292");
  move(360,100,0);

  delay(2000);
  Serial.println("225");
  move(225,100,0);
}

const int WHEEL_GEOMETRY = 1.00;

void move2 (float x, float y, float r){
  // x + is forward, y + is 
  float front_leftv = (x - y - r * WHEEL_GEOMETRY);
  float front_rightv = (x + y + r * WHEEL_GEOMETRY);
  float back_leftv = (x + y - r * WHEEL_GEOMETRY);
  float back_rightv = (x - y + r * WHEEL_GEOMETRY);

  frontLeft.setSpeed(front_leftv);
  frontRight.setSpeed(-front_rightv);
  rearLeft.setSpeed(-back_leftv);
  rearRight.setSpeed(back_rightv);
}














