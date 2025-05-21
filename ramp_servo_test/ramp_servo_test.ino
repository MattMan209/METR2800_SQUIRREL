#include <Servo.h>
Servo ramp_servo;

int count = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ramp_servo.attach(8);
  Serial.println("Steup complete");
  delay(200);


}

void loop() {
  // put your main code here, to run repeatedly:
  if(count <5){
    ramp_servo.write(80);
    delay(2000);
    ramp_servo.write(90);
    delay(2000);
    ramp_servo.write(96);
    delay(2000);
    count++;
  }
  
}
