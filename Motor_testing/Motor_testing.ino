// Motor A
 
#define pwmA 2
#define in1A 3
#define in2A 4
 
// Encoder
#define EncA 18
#define EncB 19

volatile long totalPulses = 0;

void countPulses(){   
  if(digitalRead(EncB) > 0){  // Not sure whether this will counts cw or ccw 
    totalPulses++;
  }
}
// Motor B
 
// int pwmB = 6;
// int in1B = 7;
// int in2B = 8;
 
// Motor Speed Values - Start at zero
 
int MotorSpeed1 = 0;
int MotorSpeed2 = 0;


void setup()
 
{
  Serial.begin(9600);
  // Set all the motor control pins to outputs
 
  pinMode(pwmA, OUTPUT);
  pinMode(in1A, OUTPUT);
  pinMode(in2A, OUTPUT);

  //pinMode(pwmB, OUTPUT);
  //pinMode(in1B, OUTPUT);
  //pinMode(in2B, OUTPUT);

  Serial.println("Setup complete");

  // Encoder setup for motor A
  pinMode(EncA, INPUT);
  pinMode(EncB, INPUT);


  // Interrupts can only be triggered on pins 2,3,18,19,20,21
  // Triggers an interrupt on the rising edge of the EncA
  attachInterrupt(digitalPinToInterrupt(EncA), countPulses, RISING);



}
 
void loop() {
  Serial.println(totalPulses);
  // Set Motor A forward
  digitalWrite(in1A, HIGH);
  digitalWrite(in2A, LOW);

  // SeT Motor A Backward
  //digitalWrite(in1A, LOW);
  //digitalWrite(in2A, HIGH);

  
  // Set PWM 
  
  MotorSpeed1 = 255;  //Value from 0 - 255
  analogWrite(pwmA, MotorSpeed1);



   
}