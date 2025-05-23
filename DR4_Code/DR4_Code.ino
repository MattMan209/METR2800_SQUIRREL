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
  sweep_servo.attach(servo1Pin);
  ramp_servo.attach(servo2Pin);

  //Limit Switch Setup use internal pullup
  pinMode(FRPin, INPUT_PULLUP);
  pinMode(BRPin, INPUT_PULLUP);
  pinMode(BLPin, INPUT_PULLUP);
  pinMode(FLPin, INPUT_PULLUP);

  // Initial positions
  sweep_servo.write(180);
  ramp_servo.write(90);
  
  Serial.println("Setup complete");

}

void loop() {
  //Testing Edge Tracking
  //while(1){};
  
  move(0,0,0);
  delay(3000);
  //Run track edge 500 times   
  driveLeftEdge(500, 100);

  move(0,0,0);
  while(1){};
  
  Serial.println("Forward");
  move(0,100,0);
  delay(5050);
  sweep_servo.write(95);
  delay(500);
  sweep_servo.write(70);

  stopOnEdge(); // drive forward until anylimit switch is activated. Should stop near edge
  //delay(800);  // This must make a total of 6350 with previous delay
  move(0,0,0);
  ramp_servo.write(80); //This might need to be 100. Does 90 need to be called after?
  delay(500);    // safety delay
  Serial.println("left");
  //move(-90,100,0); // Drive left hard coded.
  //delay(5000);

  driveLeftEdge(500,100); //Need to change first number until right distance
  move(0,0,0); //Stop
  delay(500);
  Serial.println("Drive Backwards to drop off");
  move(180,100,0);
  delay(3000); //Adjust to stop at deposit zone.
  move(0,0,0);
  
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

void stopOnEdge() {
  //Goes forward until any pins read Low(off edge)
  int FLState = digitalRead(FLPin);
  int BLState = digitalRead(BLPin);
  int FRState = digitalRead(FRPin);
  int BRState = digitalRead(BRPin);

  if (FLState && BLState && FRState && BRState) {move(0,100,0);}
  else {move(0,100,0);}
  }
}

void edgeTrackLeft() {
  //Track the left Edge using limit switches

  //Read sensor states
  int FL = digitalRead(FLPin); // Front Left
  int RL = digitalRead(BLPin); // Back Left
  int FR = digitalRead(FRPin); // Front Right
  int RR = digitalRead(BRPin); // Back Right

  if (!FL && !FR && !RL && !RR) { 
    //All on edge - Move 315degrees | No Rotation
    //------------
    //  |x      x|
    //  |x      x|
    Serial.println("All on board - Move 315degrees | No Rotation");
    move(315,100,0);}

  else if (FL && !FR && !RL && !RR) { 
    //Front Left Off - Move 247.5degrees | Anti Clockwise Slow
    //  |o   ------
    //------    x|
    //  |x      x|
    Serial.println("Front Left Off - Move 247.5degrees | Anti Clockwise Slow");
    move(248,100,0.3);}

  else if (FL && FR && !RL && !RR) { 
    //Perfectly Aligned - Move 270degrees | No Rotation
    //  |o      o|
    //------------
    //  |x      x|
    Serial.println("Perfectly Aligned - Move 270degrees | No Rotation");
    move(270,100,0);}

  else if (!FL && FR && !RL && !RR) { 
    //Front Right Off - Move 292degrees | Clockwise Slow
    //-------   o|
    //  |o   --------
    //  |x      x|
    Serial.println("Front Right Off - Move 292degrees | Clockwise Slow");
    move(292,100,-0.3);}

  else if (FL && !FR && RL && !RR) { 
    //All left off - Move 225 degrees | Anti Clockwise Fast
    //  |o    -   x|
    //  |o   -    x|
    Serial.println("All left off - Move 225 degrees | Anti Clockwise Fast");
    move(225,100,0.6);}

  else if (!FL && FR && !RL && RR) { 
    //All right off - Move 315 degrees | Clockwise Fast
    //  |x   -    o|
    //  |x    -   o|
    Serial.println("All right off - Move 315 degrees | Clockwise Fast");
    move(315,100,-0.6);}

  else if (FL && FR && RL && RR) { 
    //All Off - Move 225 degrees | No Rotation
    //  |0        o|
    //  |0        o|
    //----------------
    Serial.println("All Off - Move 225 degrees | No Rotation");
    move(225,100,0);}
  
  else {
    //Unknown Orientation - Backwards | No Rotation
    Serial.println("Unknown Orientation - Backwards | No Rotation");
    move(180,100,0);}
}

void driveLeftEdge(int cycles, int cycleDelay) {
  //Drive along the left edge with n amount of cycles with m delay between each state check
  for (int i = 0; i < cycles; i++){
    edgeTrackLeft();
    delay(cycleDelay);
    Serial.print(i);
    Serial.println();
  }
  move(0,0,0);
}
