#include <Servo.h>

Servo servo1;

const int analogRain = A0;    //Analog in pin of rain sensor  

int rainValue;      //Value of the rain sensor

int pos1=10;         //Open visor position
int pos2=100;        //Closed visor position

void setup() {
  servo1.attach(9);    // attaches the servo1 on pin 9 to the servo object 
  Serial.begin(9600);   // initialize serial communications at 9600 bps:
}

void loop() {
  
  rainValue = analogRead(analogRain);    
  
  Serial.print("sensor = ");              
  Serial.println(rainValue);

  if(rainValue<600){
    servo1.write(pos1);
    Serial.println("humide");
    delay(100);
  }
  else{
    servo1.write(pos2);
    Serial.println("sec");
    delay(100);
  }

  int servoAngle = servo1.read();
  Serial.print("servo Angle = ");              
  Serial.println(servoAngle);
  
  delay(200);             // wait 200 milliseconds before the next loop for the analog-to-digital

}
