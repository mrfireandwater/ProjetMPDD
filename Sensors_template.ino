#include <Servo.h>

Servo servo1;     //Wight visor (inner visor)
Servo servo2;     //Sun visor (outer visor)

const int analogRain = A0;  //TODO  //Analog in pin of rain sensor  
const int analogDist = A1;  //TODO  //Analog in pin of distance sensor  
const int speedPin = 11;    //TODO  //In pin of speed sensor  
const int lightPin = 12;    //TODO  //In pin of light sensor 

int rainValue, distValue, speedValue, lightValue;      //Value of the different sensors
int rainLimit, distLimit, speedLimit, lightLimit;      //Limit of the different sensors

int openPos=60;     //TODO    //Open visor position
int closePos;                 //Closed visor position

int r ;         //TODO: rayon de la courbe des lunettes   

void setup() {
  servo1.attach(9);     //TODO: mettre la bonne pin    // attaches the servo1 on pin 9 to the servo object 
  servo2.attach(10);    //TODO: mettre la bonne pin    // attaches the servo2 on pin 10 to the servo object 
  Serial.begin(9600);   // initialize serial communications at 9600 bps:
}

void loop() {
  
  rainValue = analogRead(analogRain);
  distValue = analogRead(analogDist);
  speedValue = // TODO;
  lightValue = //TODO;

  rainLimit = 600;       //<600:RAIN, >600:NO RAIN
  speedLimit = //TODO;   //>lightlimit:WIND, <lightLimit<NO WIND
  lightLimit = //TODO;   //>lightlimit:SUN, <lightLimit<NO SUN

  closePos = openPos + distValue*360/(2*M_PI*r);

  //Rain+Sun+Wind or Rain+Sun or Wind+Sun
  if(rainValue<rainLimit && (lightValue>lightLimit || speedValue>speedLimit)){
    servo1.write(closePos);
    servo2.write(closePos);
    delay(100);
  }
  //No Sun: Rain+Wind or Rain or Wind
  if(lightValue<lightLimit && (lightValue>lightLimit || speedValue>speedLimit)){
    servo1.write(closePos);
    servo2.write(openPos);
    delay(100);
  }
  //Sun, No Wind, No Rain
  if(lightValue>lightLimit && speedValue<speedLimit && rainValue>rainLimit){
    servo1.write(openPos);
    servo2.write(closePos);
    delay(100);
  }
  //No Sun, No Wind, No Rain
  else{
    servo1.write(openPos);
    servo2.write(openPos);
    delay(100);
  }
  
  delay(200);             // wait 200 milliseconds before the next loop for the analog-to-digital

}
