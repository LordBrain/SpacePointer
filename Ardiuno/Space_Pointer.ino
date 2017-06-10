// Most of the serial communication code was used from this example:
// http://forum.arduino.cc/index.php?topic=225329.msg1810764#msg1810764
// Compass logic was from here:
// https://github.com/jarzebski/Arduino-HMC5883L

#include <Stepper.h>
#include <Servo.h> 
#include <Wire.h> //I2C Arduino Library
#include <HMC5883L.h> //Compass Library

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

// change this to the number of steps on your motor
#define STEPS 200

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 8, 9, 10, 11);

//Servo
int servoPin = 5; 
Servo Servo1; 
boolean servoRev = true; // If servo is mounted upside down, set to true.

// Inboard LED to blink:
int led = 13;

// Reading Serial stuff
const byte buffSize = 15;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

char messageFromPC[buffSize] = {0};
float stepperLocation = 0;
float servoLocation = 0;

// Compass stuff
HMC5883L compass;
int offX = 88; //Compass off sets
int offY = 113;
float heading;
float current;

void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(38400);
  Wire.begin();
  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(60);

  // Servo
  Servo1.attach(servoPin); 

  // Compass stuff
  // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(offX, offY);

  // tell the PC we are ready
  Serial.println("<Arduino is ready>");
}

void loop(){
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  
  // Read serial input and parse
  getDataFromPC();

  // hard code point to north for now
  //stepperLocation = 0.0;
  //servoLocation = 90;
 
  // move stepper
  moveStepper(stepperLocation);
  
  // move servo
  moveServo(servoLocation);

  // Little delay for the compass to level set.
  //delay(500); 
}

void blinker(boolean blinker) {
  if(blinker){
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  else{
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  }
}


float direction() {
  blinker(true);

  Vector norm = compass.readNormalize();

  // Calculate heading
  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Minneapolis / USA declination angle is 0'18E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (0.0 + (18.0 / 60.0)) / (180 / M_PI);
  heading += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0)
  {
    heading += 2 * PI;
  }

  if (heading > 2 * PI)
  {
    heading -= 2 * PI;
  }

  // Convert to degrees
  float headingDegrees = heading * 180/M_PI; 

  // Output
  Serial.print("Heading Degress = ");
  Serial.print(headingDegrees);
  Serial.println();
  return headingDegrees;
 
  blinker(false);
}

void getDataFromPC() {
  // receive data from PC and save it into inputBuffer  
  //Serial.print("Waiting for data from serial input");
  //Serial.println();
  
  if(Serial.available() > 0) {
    blinker(true);
    char x = Serial.read();

      // the order of these IF clauses is significant
      
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      blinker(false);
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
   blinker(false);
  }
}

void parseData() {
  blinker(true);
  // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(inputBuffer,","); // this continues where the previous call left off
  stepperLocation = atof(strtokIndx);     // convert this part to a float
  
  strtokIndx = strtok(NULL, ","); 
  servoLocation = atof(strtokIndx);     // convert this part to a float
  blinker(false);
  Serial.print("Parse Data Stepper: ");
  Serial.print(stepperLocation);
  Serial.print(" Parse Data Servo: ");
  Serial.print(servoLocation);
  Serial.print(strtokIndx);
  Serial.println();
}

void moveStepper(float target){
  blinker(true);
  // read compass
  current = direction();
  float lowTarget = target - 1;
  float highTarget = target + 1;
  // loop until we get within 2 degrees of target. its good enough for me.
  while(!(current >= lowTarget && current <= highTarget)){
    // subtract current degree from target
    float numDegree = current - target;
    if (numDegree >=180 || numDegree <= -180){
       numDegree = numDegree - 180;
       numDegree = numDegree * -1; //Switches the value so the stepper goes a shorter distance
    }
    // figure out how many steps
    // Need to factor in the gear. normally its 200 steps for 360, but now its 600. 360/600=.6
    float numSteps = numDegree * .6;
    //Serial.print("Stepper Degrees: ");
    //Serial.print(numDegree);
    //Serial.print(" Stepper Steps: ");
    //Serial.print(numSteps);
    //Serial.println();
  
    stepper.step(numSteps);
    current = direction();
    delay(500);
  }
  blinker(false);
}

void moveServo(int servoLoc){
  blinker(true);
  servoLoc = 90 + servoLoc; // Data comes is as +90 or -90 from the pi.
  if(servoRev){
    servoLoc =  180 - servoLoc;
  }
  Servo1.write(servoLoc); 
  blinker(false);
}

