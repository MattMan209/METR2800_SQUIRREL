#include <Arduino.h>
#include <QTRSensors.h>
#include <Servo.h>

//#include <Servo.h>
// class declaration
QTRSensors qtr;
Servo servo1;

// declarations here
unsigned int SensorReadingCound;
const uint8_t SensorCount = 4;
uint16_t sensorReadings[SensorCount]; //main sensor reading values
uint16_t sensorCalibratedValue[SensorCount];
uint16_t sensorValuesArray[SensorCount][3];
uint32_t servo1pos = 0;

// put function declarations here:
bool edgeEstimate(uint16_t sensorReadings[4]);
//uint16_t ValueAddArray(uint16_t sensorReadings[4],uint16_t sensorValuesArray[4][3]);
int32_t edgeCalibration(bool Run, bool Use);
int32_t edgeCalibrationType2(bool Run, bool Use);
int32_t servoRun(int32_t startDegrees, int32_t endDegrees, int32_t delayTime);

// Default calibration values, replaced if use calibration data
uint16_t maxVals[SensorCount] = {980, 980, 970, 990}; 
//int pos = 


void setup() {
  // start serial communication
  Serial.begin(9600);


  // configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3}, SensorCount);
  qtr.setDimmingLevel(0);
  qtr.setEmitterPin(52);


  //Set up servo's
  servo1.attach(12);

  edgeCalibration(1,1);
  
}

void loop() {
  // read raw sensor values
  qtr.read(sensorReadings);

  //run edge estimate 100 times
  edgeEstimate(sensorReadings);

  servoRun(0,180,30);

  if (SensorReadingCound>100){
    delay(1000);
    SensorReadingCound = 0;
  } 
  delay(10);

}

// put function definitions here:
uint16_t ValueAddArray(uint16_t sensorReadings[4],uint16_t sensorValuesArray[4][3]) {
  return 0;
}

bool edgeEstimate(uint16_t sensorReadings[4]) {
  bool sensoroff[SensorCount];
  for(uint8_t i = 0; i < SensorCount; i++){
    if (sensorReadings[i] > maxVals[i])
    {
      sensoroff[i] = 1;
    }
    else {
      sensoroff[i] = 0;
    }
    Serial.print(sensorReadings[i]);
    Serial.print('\t');
  }

  for(uint8_t i = 0; i < SensorCount; i++){
    Serial.print(sensoroff[i]);
    Serial.print('\t');
  }

  Serial.println();
  SensorReadingCound++;
  return sensoroff;
}

int32_t edgeCalibration(bool Run, bool Use){
  if (!Run) return 0;
  
  // define variables
  uint32_t readingSum[SensorCount] = {0};
  uint32_t averageReading[SensorCount];
  uint32_t calibrationReadings = 200;

  // intialising to serial to warn user to align start
  Serial.println("Calibration Start");
  delay(1000);
  for (int8_t i = 3; i >= 0; i--){
    //Serial.println("Check");
    Serial.print(i);
    Serial.print('\t');
    delay(500);
  }
  Serial.println();



  // read 100 values and add each reading to variable
  for (uint8_t j = 0; j < calibrationReadings; j++){
    qtr.read(sensorReadings);
    for (uint8_t k = 0; k < SensorCount; k++) {
      readingSum[k] += sensorReadings[k];
      //Serial.print(sensorReadings[k]);
      //Serial.print('\t');
    }
    delay(20);
  }
  

  // divide avg value of each by the number of readings
  Serial.println("Calibration values:");
  for (uint8_t l = 0; l < SensorCount; l++) {
    //Serial.print(readingSum[l]);
    //Serial.print('\t');
    averageReading[l] = (readingSum[l]/calibrationReadings);  
    
    Serial.print(averageReading[l]);
    Serial.print('\t');
    if (Use)
    {
      maxVals[l] = averageReading[l] + 8;
    }
    
  }
  delay(1000);
  Serial.println();
  Serial.println("Program Start In:");
  delay(1000);
  for (int8_t i = 3; i >= 0; i--){
    //Serial.println("Check");
    Serial.print(i);
    Serial.print('\t');
    delay(800);
  }
  Serial.println();
  return 0;
}

int32_t edgeCalibrationType2(bool Run, bool Use){
  if (!Run) return 0;

  uint32_t readingSum[SensorCount] = {0};
  uint32_t readingPositions = 3;
  uint32_t positionAverages[readingPositions+2] = {0};
  uint32_t calibrationReadings = 200;

  // intialising to serial to warn user to align start
  Serial.println("Calibration Start");
  delay(1000);
  for (int8_t i = 5; i >= 0; i--){
    Serial.print(i);
    Serial.print('\t');
    delay(500);
  }
  Serial.println();
}

int32_t servoRun(int32_t startDegrees, int32_t endDegrees, int32_t delayTime){
  Serial.println("Starting Servo");
  for (servo1pos = startDegrees; servo1pos <= endDegrees; servo1pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(servo1pos);              // tell servo to go to position in variable 'pos'
    delay(delayTime);                       // waits 15ms for the servo to reach the position
  }
  for (servo1pos = endDegrees; servo1pos >= startDegrees; servo1pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(servo1pos);              // tell servo to go to position in variable 'pos'
    delay(delayTime);                       // waits 15ms for the servo to reach the position
  }
}