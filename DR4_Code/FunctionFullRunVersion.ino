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

  // Initialise wheels
  frontLeft.init();
  frontRight.init();
  rearLeft.init();
  rearRight.init();
  
  // STBY pins
  digitalWrite(stby, HIGH);
  digitalWrite(stby2, HIGH);
  
  // Attatch interrupts 
  attachInterrupt(digitalPinToInterrupt(18), FL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(20), FR_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(2), RL_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(18), RR_ISR, RISING); // Adjust pin if needed

  //Servo setup
  sweep_servo.attach(servo2Pin);
  ramp_servo.attach(servo1Pin);

  //Limit switch setup, use internal pullup States negates, fixed in implementation of loops
  pinMode(FRPin, INPUT_PULLUP);
  pinMode(BRPin, INPUT_PULLUP);
  pinMode(BLPin, INPUT_PULLUP);
  pinMode(FLPin, INPUT_PULLUP);

  // Initial positions
  sweep_servo.write(90); 
  ramp_servo.write(90);
  
  Serial.println("Setup complete");
}

void loop() {
  //Start, write servos to their positions
  //Drop ramp servo slightly
  runStart(170);

  //Do forward movement and sweep balls
  Serial.println("Forward");
  pickUpBalls(100,5050,70); //Forward speed, forward time, sweep angle

  //Wait until the robot hits the edge
  waitForEdge();
  reverseOffEdge(60,150);

  //Driving down the left edge
  delay(200);
  driveLeftEdge(1910, 10);
  move(0,0,0);
  delay(200);

  //Reverse to hit deposit zone
  reverseToDeposit(100, 3500); //Reverse speed, reverse time

  //Lift up ramp to drop balls
  ramp_servo.write(160);
  delay(1500);

  // Relax sweep to stop sticking of ramp
  sweep_servo.write(90);
  delay(100);

  //Shake to make sure all the balls come out.
  ramp_servo.write(140);
  delay(600);
  shakeRamp(4);

  //Drive off Deposit Zone
  move(0,100,0);
  delay(300);
  move(0,0,0);

  // Lift Sweep to demonstrate finish
  sweep_servo.write(180);
  move(0,0,0);

  //end
  while(1){};
}

void move(float angle_deg, float max_wheel_speed, float rotation_speed) {
  //Original move function which cannot solve rotations. Changed to Move2.
  //Rotations do not work
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
  //Used for testing and debugging sensor states
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
  //Repeats system edge conditions until they are met
  while (stopOnEdge() == 1) {
  }
}

int stopOnEdge() {
  //Drives forward until any limit switches open
  //Brakes once edge conditions are met to step system
  //Drives forward at speed 80
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

const int slowRotationSpeed = 7; //Speed to rotate when one sensor off     ||was 5 
const int fastRotationSpeed = 12; //Speed to rotate when two sensors are off

const int ySpeed = 80; //Speed to travel left
const int xSpeedSlow = 5; //Speed to correct small angle
const int xSpeedFast = 15; //Speed to correct large angle

void edgeTrackLeft() {
  //Drives down the left edge of the track using 4 limit switches as a sensor imputs

  //Reads sensor states, checks if any expected sensor combinations are met, move in corrective direction and rotation.
  //Sensor and robot positions are shown in comments

  //If no expected sensor combinations are found, robot moves backward.

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
  //Calls the edgeTrackLeft function set quantity with a set delay
  //Runs the loop until inline with deposit zone then brakes
  
  for (int i = 0; i < cycles; i++){
    edgeTrackLeft();
    delay(cycleDelay);
    Serial.print(i);
    Serial.println();
  }
  move(0,0,0);
}

void rotateInPlace(float speed) {
  // FUNCTION NOT USED
  // Debugging function
  // speed: positive for counterclockwise (left), negative for clockwise (right)
  // speed range: -max_wheel_speed to +max_wheel_speed

  //move(0,100,0.2);

  frontLeft.setSpeed(speed);
  frontRight.setSpeed(-speed);
  rearLeft.setSpeed(speed);
  rearRight.setSpeed(-speed);
}

void movementTest(){
  //Function not used
  // Debugging movement to check motos and wheels operational.


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
  // Drives system in vector of combination of x and y
  // Positive r+ is anticlockwise rotation

  // x+ is forward, y+ is left 
  float front_leftv = (x - y - r * WHEEL_GEOMETRY);
  float front_rightv = (x + y + r * WHEEL_GEOMETRY);
  float back_leftv = (x + y - r * WHEEL_GEOMETRY);
  float back_rightv = (x - y + r * WHEEL_GEOMETRY);

  frontLeft.setSpeed(front_leftv);
  frontRight.setSpeed(-front_rightv);
  rearLeft.setSpeed(-back_leftv);
  rearRight.setSpeed(back_rightv);
}


void runStart(int SweepAngle) {
  //Starts the requirement run
  //Sets servo required positions
  delay(2000);
  sweep_servo.write(SweepAngle);
  delay(200);
  ramp_servo.write(60);
  delay(200);
  ramp_servo.write(90);
}


void pickUpBalls(int forwardSpeed, int forwardTime, int sweepAngle){
    //Drives forward for set time then closes sweep to collect balls
    move2(forwardSpeed,0,0);
    delay(forwardTime);
    sweep_servo.write(sweepAngle); //Shut Sweep
    delay(100);
  }


void reverseOffEdge(int reverseSpeed, int reverseTime){
    // Reverses off the edge a set time and distance
    // call once edge conditions are met
    move2(0,0,0);
    delay(200);
    //move(180,60,0);//move back off edge slightly 150ms
    move2(-reverseSpeed,0,0);
    delay(reverseTime); //this worked at 300 maybe go back
    move2(0,0,0);
  }

void reverseToDeposit(int reverseSpeed, int reverseTime){
    // Reverse system after travelling down the left edge to the deposit zone
    // Reverses at a set speed and time then stops
    move2(-reverseSpeed,0,0);
    delay(reverseTime);
    move(0,0,0);
    delay(200);
  }

void shakeRamp(int numShakes){
    // rapidly raise and lower ramp to dislodge any stuck balls
    for(int i = 0; i < numShakes; i++) {
      ramp_servo.write(80);
      delay(100);
      move2(20,0,0);
      ramp_servo.write(130);
      delay(300);
      move2(-20,0,0);
    }
    ramp_servo.write(90);
  }









