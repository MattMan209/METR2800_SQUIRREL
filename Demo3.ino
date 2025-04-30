#include <AccelStepper.h>
#include <Servo.h>



int pos = 180;
int pos2 = 0;
int stopValue = 90; 
 

// Define pin connections
//stepper pins
const int dirPin = 22;
const int stepPin = 7;

//Servo Pins
const int servo1Pin = 9;
const int servo2Pin = 8;


// Define stepper motor interface type
#define motorInterfaceType 1

// Create objects
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);
Servo myservo1; //arm servo
Servo myservo2; //spool servo

void setup() {
	myservo1.attach(servo1Pin); 
	myservo1.write(0);
	myservo2.attach(servo2Pin);
	myservo2.write(stopValue);
	// set the maximum speed of the stepper motor steps
	myStepper.setMaxSpeed(2000);
	// sert the maximum acceleration of stepper
	myStepper.setAcceleration(1000);
}

void loop() {
	//while (true){delay(1000);};
	for(pos = 0; pos <= 110; pos += 1) { // goes from 0 degrees to 110 degrees
    // in steps of 1 degree
    myservo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       
  }

	//Run Stepper motor from Starting position to where to stop ball.
	myStepper.runToNewPosition(0);
	delay(1000);
	myStepper.runToNewPosition(16000);
	delay(1000);
	//myStepper.runToNewPosition(0);


	//while (true){delay(1000)};
	
	//Servo ramp slight spin
	myservo2.write(80); // Slight backward
  // Reverse
	delay(3000);
  myservo2.write(96); // Slight forward
  delay(900);
  // Stop servo
  myservo2.write(stopValue);
  delay(5000);
	

	//run stepper and servo back
	myStepper.runToNewPosition(0);
	delay(100);
	myservo1.write(0);
	delay(100);
  while (true){delay(1000);};

}