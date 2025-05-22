#include <Servo.h>
Servo ramp_servo;
Servo arm_servo;

int count = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ramp_servo.attach(5);
  arm_servo.attach(4);
  delay(200);
  arm_servo.write(180);           //initial 0 
  ramp_servo.write(90);
 

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("run loop");

  ramp_servo.write(50);
  delay(2000);
  ramp_servo.write(90);
  delay(2000);
  ramp_servo.write(120);
  delay(2000);
  ramp_servo.write(90);

  
  while(1){};
  /*
  delay(3000);
  ramp_servo.write(100);
  delay(3000);
  ramp_servo.write(90);
  delay(3000);
  
  delay(2000);
  ServoDown();
  delay(2000);
  arm_servo.write(0);
  delay(3000);
  ServoDown();
  arm_servo.write(0);
  while (1){}
  */
  
}

void ServoDown() {
  for(int pos = 0; pos <= 110; pos += 1) { // goes from 0 degrees to 110 degrees  
    arm_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       //550ms total round to 560ms with calls
  }
}