
// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include "src/RGBConverter/RGBConverter.h"




// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10



const int RGB_RED_PIN = 9;
const int RGB_GREEN_PIN  = 10;
const int RGB_BLUE_PIN  = 11;
const boolean COMMON_ANODE = false; 



const byte MAX_RGB_VALUE = 255;

const int BUTTON_INPUT_PIN = 5;
const int LED_OUTPUT_PIN = 3;
const int DEBOUNCE_WINDOW = 40; // in milliseconds

int _prevRawButtonVal = LOW;
int _debouncedButtonVal = LOW;
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the input pin was toggled


float hue = 0.5; // 0 - 360
float saturation = 1;
float lightness = 0.5;

RGBConverter _rgbConverter;



// software SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);
// hardware SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();




void setup(void) {
  Serial.begin(9600);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Serial.println("LIS3DH test!");

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  // Serial.println("LIS3DH found!");

  // lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  // Serial.print("Range = "); Serial.print(2 << lis.getRange());
  // Serial.println("G");

  // lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  // Serial.print("Data rate set to: ");
  // switch (lis.getDataRate()) {
  //   case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
  //   case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
  //   case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
  //   case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
  //   case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
  //   case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
  //   case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

  //   case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
  //   case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
  //   case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  // }
}

void loop() {
  lis.read();      // get X Y and Z data at once
  checkAndParseSerial();
  buttonData();
  updateLight();
  sendData();
  delay(100);
}

void checkAndParseSerial() {

  if (Serial.available() > 0) {
    String rcvdSerialData = Serial.readStringUntil('\n');

    // Serial.print("#recieved: ");
    // Serial.println(rcvdSerialData);

    String newHue = getValue(rcvdSerialData, ',', 0);
    String newSaturation = getValue(rcvdSerialData, ',', 1);
    String newLightness = getValue(rcvdSerialData, ',', 2);

    if (newHue.toFloat() > 1 || newHue.toFloat() < 0) {
      Serial.println("#out of bounds hue");
    }

    hue = newHue.toFloat();
    saturation = newSaturation.toFloat();
    lightness = newLightness.toFloat();



    // String output = "#recieved:" + hue + "," + saturation + "," + test;
    // Serial.println(output);
  

  }


}

// from crossfade section of textbook
void updateLight() {
    // Convert current hue, saturation, and lightness to RGB
  // The library assumes hue, saturation, and lightness range from 0 - 1
  // and that RGB ranges from 0 - 255
  // If lightness is equal to 1, then the RGB LED will be white
  byte rgb[3];

  float saturation = 1;
  float lightness = 0.5;

  _rgbConverter.hslToRgb(hue, saturation, lightness, rgb);
  setColor(rgb[0], rgb[1], rgb[2]); 

}

/**
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue. 
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 * 
 */
void setColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_RGB_VALUE - red;
    green = MAX_RGB_VALUE - green;
    blue = MAX_RGB_VALUE - blue;
  }
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}

// by Odis Harkins from https://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string 
String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// debounce code from debounce section of textbook
void buttonData() {
   // Read the button value. We assume a pull-down resistor button configuration so
  // the button will be HIGH when pressed and LOW when not pressed
  int rawButtonVal = digitalRead(BUTTON_INPUT_PIN);

  // If the button state has changed since the last reading, grab timestamp
  // This state change may be due to a legitimate user action or noise
  if(rawButtonVal != _prevRawButtonVal){
    _buttonStateChangeTimestamp = millis();
  }

  if((millis() - _buttonStateChangeTimestamp) >= DEBOUNCE_WINDOW){
    // If we're here, then the button state hasn't changed for at least DEBOUNCE_WINDOW
    // So, we must be in a steady state

    _debouncedButtonVal = rawButtonVal;
  }


  _prevRawButtonVal = rawButtonVal;
}



// "x,y,z,x-acc,y-acc,z-acc, isButtonPressed"
void sendData() {
  sensors_event_t event;
  lis.getEvent(&event);

  float data[] = {lis.x, lis.y, lis.z, event.acceleration.x, event.acceleration.y, event.acceleration.z, _debouncedButtonVal};
  const int DATA_LENGTH = 7;

  String output = "";
  for (int i = 0; i < DATA_LENGTH; i++) {
    output += String(data[i]);
    if (i + 1 != DATA_LENGTH) output += ",";
  }

  Serial.println(output);
}
