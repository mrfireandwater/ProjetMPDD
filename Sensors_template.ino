#include <Average.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Servo.h>
#include "Adafruit_TSL2591.h"
#include "Adafruit_VL6180X.h"

#define TCAADDR 0x70

// Reserve space for 10 entries in the average bucket.
// Change the type between < and > to change the entire way the library works.
Average<float> ave(10);

Servo servo1;     //Wight visor (inner visor)
Servo servo2;     //Sun visor (outer visor)

const int analogRain = A0;  //TODO  //Analog in pin of rain sensor  
const int analogDist = A1;  //TODO  //Analog in pin of distance sensor  
const int speedPin = 11;    //TODO  //In pin of speed sensor


/*
 * rainLimit def    : indicator of quantity of water on the capacitive sensor.
 * raiLimit values  : dry sensor output value 630. sensor with water dropplets output value between 450 and 550
 * distLimit def    : distance from helmet to nose.
 * distLimit values : varies between 50 and 100mm, to be determined when the user wear it.
 * speedLimit def   : boolean value when speed threshold is reached
 * speedLimit values: 0 or 1
 * lightLimit def   : quantity of lumens to activate sunglasses
 * lightLimit value : 4000
 */
int rainValue, distValue, speedValue, lightValue;      //Value of the different sensors
int rainLimit = 600, distLimit = 50, speedLimit = 1, lightLimit = 4000;      //Limit of the different sensors

int openPos=60;     //TODO    //Open visor position
int closePos;                 //Closed visor position

int r = 100;         //TODO: rayon de la courbe des lunettes 

int analogMoisture = 0;
int analogDistanceInfrared = 1;

int distanceNose = 0;
int moistureRain = 0;
int lux = 0;
uint8_t distTof = 0;


Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)
Adafruit_VL6180X vl = Adafruit_VL6180X();

/**************************************************************************/
/*
    Configures the gain and integration time for the lux TSL2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}

/**************************************************************************/
/*
    Shows how to perform a basic read on visible, full spectrum or
    infrared light (returns raw 16-bit ADC values)
*/
/**************************************************************************/
int simpleRead()
{
  tcaselect(1); // call the multiplexer to switch the I2C bus on this sensor
  // Simple data read example. Just read the infrared, fullspecrtrum diode 
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
int x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  //Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  //Serial.print(F("Luminosity: "));
  //Serial.println(x, DEC);
  return x;
}

/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
void advancedRead(void)
{
  tcaselect(1); // call the multiplexer to switch the I2C bus on this sensor
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir), 6);
}

uint8_t tof(){
  tcaselect(0); // call the multiplexer to switch the I2C bus on this sensor
  //float lux = vl.readLux(VL6180X_ALS_GAIN_5);

  //Serial.print("Lux: "); Serial.println(lux);
  
  uint8_t range = vl.readRange();
  //uint8_t status = vl.readRangeStatus();

  return range;
}

//multiplexer
// tutorial: https://learn.adafruit.com/adafruit-tca9548a-1-to-8-i2c-multiplexer-breakout/wiring-and-test
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  servo1.attach(9);     //TODO: mettre la bonne pin    // attaches the servo1 on pin 9 to the servo object 
  servo2.attach(10);    //TODO: mettre la bonne pin    // attaches the servo2 on pin 10 to the servo object 

  // declare ADC inputs
  pinMode(analogMoisture, INPUT);
  pinMode(analogDistanceInfrared, INPUT);  

  pinMode(2, INPUT_PULLUP); //input of speed sensor.
  
  tcaselect(0);
  Serial.println("Adafruit VL6180x test!");
  if (!vl.begin()) {
    Serial.println("Failed to find ToF sensor");
  }
  else{
    Serial.println("ToF Sensor found!");
  }

  Serial.println("Adafruit lux TSL2591 test!");
  tcaselect(1);
  if (tsl.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No lux sensor found ... check your wiring?"));
  }
  configureSensor(); //lux
}

void loop() {

  rainValue = analogRead(analogRain);
  distValue = tof(); //used with the I2C time of flight sensor
  //distValue = analogRead(analogDist); //used with the infrared sharp sensor
  speedValue = digitalRead(2);// TODO
  lightValue = simpleRead();

  rainLimit = 600;       //<600:RAIN, >600:NO RAIN
  speedLimit = 0;   //>lightlimit:WIND, <lightLimit<NO WIND
  lightLimit = 4000;   //>lightlimit:SUN, <lightLimit<NO SUN

  closePos = openPos + distValue*360/(2*M_PI*r);

  //Rain+Sun+Wind or Rain+Sun or Wind+Sun
  if(rainValue<rainLimit && (lightValue>lightLimit || speedValue>speedLimit)){
    servo1.write(closePos);
    servo2.write(closePos);
    Serial.println("Rain+Sun+Wind or Rain+Sun or Wind+Sun");
    delay(100);
  }
  //No Sun: Rain+Wind or Rain or Wind
  else if(lightValue<lightLimit && (lightValue>lightLimit || speedValue>speedLimit)){
    servo1.write(closePos);
    servo2.write(openPos);
    Serial.println("No Sun: Rain+Wind or Rain or Wind");
    delay(100);
  }
  //Sun, No Wind, No Rain
  else if(lightValue>lightLimit && speedValue<speedLimit && rainValue>rainLimit){
    servo1.write(openPos);
    servo2.write(closePos);
    Serial.println("Sun, No Wind, No Rain");
    delay(100);
  }
  //No Sun, No Wind, No Rain
  else{
    servo1.write(openPos);
    servo2.write(openPos);
    Serial.println("No Sun, No Wind, No Rain");
    Serial.println("no condition matched");
    delay(100);
  }
  
  delay(500);             // wait 200 milliseconds before the next loop for the analog-to-digital
  Serial.print("Tof: ");
  Serial.print(distValue);
  Serial.print(", Lux: ");
  Serial.print(lightValue);
  Serial.print(", Sharp distance: ");
  Serial.print(rainValue = analogRead(1)); //TODO
  Serial.print(", Speed threshold: ");
  Serial.print(speedValue);
  Serial.print(", Moisture: ");
  Serial.println(rainValue);
  
  
}
