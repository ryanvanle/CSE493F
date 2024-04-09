// Author: Ryan Le
// Used code from CITE SOURCES HERE

#include "src/RGBConverter/RGBConverter.h"

// circuit settings
const boolean COMMON_ANODE = false; 
const boolean PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER = true; // set false if photocell is R1

// pin settings
const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int PHOTOCELL_INPUT_PIN = A0;
const int BUTTON_INPUT_PIN = 8;
const int ROTATION_INPUT_PIN = A1;

// button mode settings
const int DEBOUNCE_WINDOW = 100;
int _prevRawButtonVal = LOW;
int _debouncedButtonVal = LOW;
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the input pin was toggled

// photocell sensor settings
const int MIN_PHOTOCELL_VAL = 550; // turn on LED
const int MAX_PHOTOCELL_VAL = 900; // fully on value

// LED settings
const int DELAY_INTERVAL = 50; // interval in ms between incrementing hues
const byte MAX_RGB_VALUE = 255;
float _hue = 0; //hue varies between 0 - 1
float _step = 0.001f;
RGBConverter _rgbConverter;

int photocellValue = 0;

// rotation


// general settings
const float MAX_MODES = 3;
int currentMode = 1;

void setup() {
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT);

  Serial.begin(9600);
}

void loop() {

  updateMode();

  if (currentMode == 1) {
    updatePhotocell();
  } else if (currentMode == 2) {
    updateRotation();
  } else if (currentMode == 3) {
    // tba; 
  }

  // Serial.print(currentMode);
  // Serial.print(",");
  // Serial.println(_debouncedButtonVal);
}



const int MAX_VOUT = 1000;
const int MIN_VOUT = 100;
void updateRotation() {


  int vOut = analogRead(ROTATION_INPUT_PIN);
  photocellValue = analogRead(PHOTOCELL_INPUT_PIN);

  double rotationHue = map(vOut, MIN_VOUT, MAX_VOUT, 0, 100) * 0.01;
  float saturation = 1;
  // float lightness = map(x/zphotocellValue, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 5) * 0.001;
  float lightness = 0.1;

  
  // if (rotationHue > 1.0) {
  //   rotationHue -= 1;
  // }

  Serial.print(vOut);
  Serial.print(",");
  Serial.println(rotationHue);

  byte rgb[3];
  _rgbConverter.hslToRgb(rotationHue, saturation, lightness, rgb);
  setColor(rgb[0], rgb[1], rgb[2]); 
  delay(DELAY_INTERVAL);
  // delay();


} 


void updatePhotocell() {
  // Read the photo-sensitive resistor value. If you have the photocell resistor hooked 
  // up as Rtop in the voltage divider (that is, one leg of the photocell is connected to 5V), 
  // then higher values correspond to brightness. If you have the photocell hooked up as Rbottom 
  // in the voltage divider (that is, one leg of the photocell is connected to GND), then
  // higher values correspond to darkness.
  photocellValue = analogRead(PHOTOCELL_INPUT_PIN);

  // Convert current hue, saturation, and lightness to RGB
  // The library assumes hue, saturation, and lightness range from 0 - 1
  // and that RGB ranges from 0 - 255
  // If lightness is equal to 1, then the RGB LED will be white
  byte rgb[3];

  // Remap the value for output. 
  float lightness = map(photocellValue, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 5) * 0.1;
  float saturation = 1;
  _rgbConverter.hslToRgb(_hue, saturation, lightness, rgb);

  setColor(rgb[0], rgb[1], rgb[2]); 

  // update hue based on step size
  _hue += _step;

  // hue ranges between 0-1, so if > 1, reset to 0
  if(_hue > 1.0){
    _hue = 0;
  }

  delay(DELAY_INTERVAL);

  // Print the raw photocell value and the converted led value (e,g., for Serial 
  // Console and Serial Plotter)
  Serial.print(photocellValue);
  Serial.print(",");
  Serial.println(lightness);
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



boolean hasPressedRecently = false;

void updateMode() {
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

    if (!hasPressedRecently && _debouncedButtonVal) {
      hasPressedRecently = true;
      currentMode++;
    } else if (!_debouncedButtonVal) {
      hasPressedRecently = false;
    }

    if (currentMode > MAX_MODES) {
      currentMode = 1;
    }

  }

  _prevRawButtonVal = rawButtonVal;
}

