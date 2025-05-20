#include <SparkFun_TB6612.h>

const int offsetA = 1;
const int offsetB = 1;

#define AIN1 25
#define BIN1 29
#define AIN2 23
#define BIN2 31
#define PWMA 10
#define PWMB 11
#define STBY 27

const int offsetC = 1;
const int offsetD = 1;

#define CIN1 47
#define DIN1 51
#define CIN2 45
#define DIN2 53
#define PWMC 13
#define PWMD 12
#define BSTBY 49

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);
Motor motor3 = Motor(CIN1, CIN2, PWMC, offsetC, BSTBY);
Motor motor4 = Motor(DIN1, DIN2, PWMD, offsetD, BSTBY);
void setup()
{
 //Nothing here
}


void loop()
{
   motor1.drive(100,1000);
   motor2.drive(100,1000);
   motor3.drive(100,1000);
   motor4.drive(100,1000);
   delay(100000000);
   //Use of the drive function which takes as arguements the speed
   //and optional duration.  A negative speed will cause it to go
   //backwards.  Speed can be from -255 to 255.  Also use of the 
   //brake function which takes no arguements.
   motor1.drive(255,1000);
   motor1.drive(-255,1000);
   motor1.brake();
   delay(1000);
   
   //Use of the drive function which takes as arguements the speed
   //and optional duration.  A negative speed will cause it to go
   //backwards.  Speed can be from -255 to 255.  Also use of the 
   //brake function which takes no arguements.
   motor2.drive(255,1000);
   motor2.drive(-255,1000);
   motor2.brake();
   delay(1000);
   
   //Use of the forward function, which takes as arguements two motors
   //and optionally a speed.  If a negative number is used for speed
   //it will go backwards
   forward(motor1, motor2, 150);
   delay(1000);
   
   //Use of the back function, which takes as arguments two motors 
   //and optionally a speed.  Either a positive number or a negative
   //number for speed will cause it to go backwards
   back(motor1, motor2, -150);
   delay(1000);
   
   //Use of the brake function which takes as arguments two motors.
   //Note that functions do not stop motors on their own.
   brake(motor1, motor2);
   delay(1000);
   
   //Use of the left and right functions which take as arguements two
   //motors and a speed.  This function turns both motors to move in 
   //the appropriate direction.  For turning a single motor use drive.
   left(motor1, motor2, 100);
   delay(1000);
   right(motor1, motor2, 100);
   delay(1000);
   
   //Use of brake again.
   brake(motor1, motor2);
   delay(1000);
   
}f
