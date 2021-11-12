#include <Servo.h>

Servo myservo;

const int analogRain = A0;    //Analog in pin of rain sensor  
const int analogDist = A1;    //Analog in pin of distance sensor  
const int speedPin = 11;      //In pin of speed sensor  
const int lightPin = 12;      //In pin of light sensor 

const int servo1 = 9;     //Controls wight visor (inner visor)
const int servo2 = 10;    //Controls sun visor (outer visor)

int rainValue, distValue, speedValue, lightValue;      //Value of the different sensors
int rainLimit, distLimit, speedLimit, lightLimit;      //Limit of the different sensors

int pos1=60;         //Open visor position
int pos2=180;        //Closed visor position

int pos;

void setup() {
  Serial.begin(9600);   // initialize serial communications at 9600 bps:
  myservo.attach(9);    // attaches the servo1 on pin 9 to the servo object 
  myservo.attach(10);    // attaches the servo2 on pin 10 to the servo object 
}

void loop() {
  
  rainValue = analogRead(analogRain);
  distValue = analogRead(analogDist);
  speedValue = // TODO;
  lightValue = //TODO;

  rainLimit = 600;      //<600:MOUILLE, >600:SEC
  /*distLimit = #TODO;
  speedLimit = #TODO;
  lightLimit = #TODO;*/   //>lightlimit:SUN, <lightLimit<NO SUN

  if(rainValue<rainLimit and lightValue>lightLimit){
    myservo.write(pos1);
    delay(100);
  }
  else{
    myservo.write(pos2);
    delay(100);
  }
  
  delay(200);             // wait 200 milliseconds before the next loop for the analog-to-digital

}
