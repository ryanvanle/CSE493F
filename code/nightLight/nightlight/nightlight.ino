// Author: Ryan Le
// No AI code used

// Used code from Physical Computing Textbook
// Sections used:
// Debouncing switches: https://makeabilitylab.github.io/physcomp/arduino/#lesson-3-debouncing-switches 
// RGB LED and hue library: https://makeabilitylab.github.io/physcomp/arduino/rgb-led.html
// Piano lesson: https://makeabilitylab.github.io/physcomp/arduino/piano.html
// Crossfading RGB LEDs: https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade.html

#include "src/RGBConverter/RGBConverter.h"

// circuit settings
const boolean COMMON_ANODE = false; 
const boolean PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER = true; // set false if photocell is R1

// pin settings
const int RGB_RED_PIN = 3;
const int RGB_GREEN_PIN  = 6;
const int RGB_BLUE_PIN  = 5;
const int PHOTOCELL_INPUT_PIN = A0;
const int MODE_BUTTON_INPUT_PIN = 7;
const int SECONDARY_BUTTON_INPUT_PIN = 8;
const int ROTATION_INPUT_PIN = A1;
const int BUZZER_OUTPUT_PIN = 12;

// photocell sensor settings
const int MIN_PHOTOCELL_VAL = 350; // turn on LED
const int MAX_PHOTOCELL_VAL = 1023; // fully on value

// LED settings
const int DELAY_INTERVAL = 5; // interval in ms between incrementing hues
const byte MAX_RGB_VALUE = 255;
float _hue = 0; //hue varies between 0 - 1
float _step = 0.001f;
RGBConverter _rgbConverter;

// button mode settings
boolean hasModePressedRecently = false;
const int DEBOUNCE_WINDOW = 40;
int _modePrevRawButtonVal = LOW;
int _modeDebouncedButtonVal = LOW;
unsigned long _modeButtonStateChangeTimestamp = 0;  // the last time the input pin was toggled

// rotation lock button settings
boolean hasLockPressedRecently = false;
int _lockPrevRawButtonVal = LOW;
int _lockDebouncedButtonVal = LOW;
unsigned long _lockButtonStateChangeTimestamp = 0;  // the last time the input pin was toggled
boolean isLocked = false;

// rotation range
const int MAX_VOUT = 1000;
const int MIN_VOUT = 100;

// general settings
const float MAX_MODES = 3;
int currentMode = 1;


// game settings
boolean isInGame = false;


void setup() {
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
  pinMode(MODE_BUTTON_INPUT_PIN, INPUT);
  pinMode(SECONDARY_BUTTON_INPUT_PIN, INPUT);
  pinMode(BUZZER_OUTPUT_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  updateMode();
  Serial.print("mode ");
  Serial.println(currentMode);

  if (currentMode == 1) {
    updatePhotocell();
  } else if (currentMode == 2) {
    updateRotation();
  } else if (currentMode == 3) {
    updateGame();
    // tba; 
  }

  // Serial.print(currentMode);
  // Serial.print(",");
  // Serial.println(_leftDebouncedButtonVal);
}

void updatePhotocell() {
  // Read the photo-sensitive resistor value. If you have the photocell resistor hooked 
  // up as Rtop in the voltage divider (that is, one leg of the photocell is connected to 5V), 
  // then higher values correspond to brightness. If you have the photocell hooked up as Rbottom 
  // in the voltage divider (that is, one leg of the photocell is connected to GND), then
  // higher values correspond to darkness.
  float photocellValue = analogRead(PHOTOCELL_INPUT_PIN);

  // Convert current hue, saturation, and lightness to RGB
  // The library assumes hue, saturation, and lightness range from 0 - 1
  // and that RGB ranges from 0 - 255
  // If lightness is equal to 1, then the RGB LED will be white
  byte rgb[3];

  // Remap the value for output. 
  float lightness = map(photocellValue, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 500) * 0.001;
  if (lightness < 0) lightness = 0;
  if (lightness > 500) lightness = 1023;


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
  // Console and Serial Plotter)]
  // Serial.print(currentMode);
  // Serial.print(",");

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


// NOTE: I could re-implement this code as a class but unfortunately I do not have the time with the confidence to do it correctly.


void updateMode() {
  // Read the button value. We assume a pull-down resistor button configuration so
  // the button will be HIGH when pressed and LOW when not pressed
  int rawButtonVal = digitalRead(MODE_BUTTON_INPUT_PIN);

  // If the button state has changed since the last reading, grab timestamp
  // This state change may be due to a legitimate user action or noise
  if(rawButtonVal != _modePrevRawButtonVal){
    _modeButtonStateChangeTimestamp = millis();
  }

  if((millis() - _modeButtonStateChangeTimestamp) >= DEBOUNCE_WINDOW){
    // If we're here, then the button state hasn't changed for at least DEBOUNCE_WINDOW
    // So, we must be in a steady state
    _modeDebouncedButtonVal = rawButtonVal;

    if (!hasModePressedRecently && _modeDebouncedButtonVal  && (millis() - _modeButtonStateChangeTimestamp) >= 1000) {
      hasModePressedRecently = true;
      isLocked = false;
      noTone(BUZZER_OUTPUT_PIN);
      isInGame = false;
      currentMode++;
    } else if (!_modeDebouncedButtonVal) {
      hasModePressedRecently = false;
    }

    if (currentMode > MAX_MODES) {
      currentMode = 1;
    }

  }

  _modePrevRawButtonVal = rawButtonVal;
}


void updateLock() {
  int rawButtonVal = digitalRead(SECONDARY_BUTTON_INPUT_PIN);

  if(rawButtonVal != _lockPrevRawButtonVal){
    _lockButtonStateChangeTimestamp = millis();
  }

  if((millis() - _lockButtonStateChangeTimestamp) >= DEBOUNCE_WINDOW){
    _lockDebouncedButtonVal = rawButtonVal;

    if (!hasLockPressedRecently && _lockDebouncedButtonVal) {
      hasLockPressedRecently = true;
      isLocked = !isLocked;
    } else if (!_lockDebouncedButtonVal) {
      hasLockPressedRecently = false;
    }
  }

  // Serial.println(isLocked);

  _lockPrevRawButtonVal = rawButtonVal;
}

void updateRotation() {

  updateLock();
  // Serial.println(isLocked);
  if (isLocked) return;
  
  int vOut = analogRead(ROTATION_INPUT_PIN);

  double rotationHue = map(vOut, MIN_VOUT, MAX_VOUT, 0, 100) * 0.01;
  float saturation = 1;
  float lightness = 0.1;

  if (rotationHue < 0) rotationHue = 0;

  Serial.print(vOut);
  Serial.print(",");
  Serial.println(rotationHue);

  byte rgb[3];
  _rgbConverter.hslToRgb(rotationHue, saturation, lightness, rgb);
  setColor(rgb[0], rgb[1], rgb[2]); 
  delay(DELAY_INTERVAL);
} 


int currentColorIndex = 0;

void updateGame() {

  // red high low low
  // green low high low
  // blue low low high;

  int colors[][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};

  if (!isInGame) {
    tone(BUZZER_OUTPUT_PIN, 262, 20);
    setColor(colors[0][0], colors[0][1], colors[0][2]); 
    delay(1000);

    tone(BUZZER_OUTPUT_PIN, 262, 20);
    setColor(colors[2][0], colors[2][1], colors[2][2]); 
    delay(1000);

    tone(BUZZER_OUTPUT_PIN, 1210, 500);
    setColor(colors[1][0], colors[1][1], colors[1][2]); 

    delay(200);
    setColor(0, 0, 0); 
    delay(1000);

    currentColorIndex = random(3);
    setColor(colors[currentColorIndex][0], colors[currentColorIndex][1], colors[currentColorIndex][2]); 

    isInGame = true;

  }

  // left red
  // right green
  // both blue

  boolean leftButtonValue = isLeftPressed();
  boolean rightButtonValue = isRightPressed();
  boolean isRed = currentColorIndex == 0;
  boolean isGreen = currentColorIndex == 1;
  boolean isBlue = currentColorIndex == 2;

  boolean isCorrectAnswer = (leftButtonValue && rightButtonValue && isBlue) || (leftButtonValue && isRed) || (rightButtonValue && isGreen);

  if (isCorrectAnswer) {
    tone(BUZZER_OUTPUT_PIN, 1210, 500);
    currentColorIndex = random(3);
    setColor(colors[currentColorIndex][0], colors[currentColorIndex][1], colors[currentColorIndex][2]); 
  }
  
}

int _leftPrevRawButtonVal = LOW;
int _leftDebouncedButtonVal = LOW;
unsigned long _leftButtonStateChangeTimestamp = 0;  // the last time the input pin was toggled
boolean isLeftPressed() {

  int rawButtonVal = digitalRead(SECONDARY_BUTTON_INPUT_PIN);

  if(rawButtonVal != _leftPrevRawButtonVal){
    _leftButtonStateChangeTimestamp = millis();
  }

  if((millis() - _leftButtonStateChangeTimestamp) >= DEBOUNCE_WINDOW){
    _leftDebouncedButtonVal = rawButtonVal;
  }

  _leftPrevRawButtonVal = rawButtonVal;
  return _leftDebouncedButtonVal == HIGH;
}

int _rightPrevRawButtonVal = LOW;
int _rightDebouncedButtonVal = LOW;
unsigned long _rightButtonStateChangeTimestamp = 0;  // the last time the input pin was toggled
boolean isRightPressed() {

  int rawButtonVal = digitalRead(MODE_BUTTON_INPUT_PIN);

  if(rawButtonVal != _rightPrevRawButtonVal){
    _rightButtonStateChangeTimestamp = millis();
  }

  if((millis() - _rightButtonStateChangeTimestamp) >= DEBOUNCE_WINDOW){
    _rightDebouncedButtonVal = rawButtonVal;
  }

  _rightPrevRawButtonVal = rawButtonVal;
  return _rightDebouncedButtonVal == HIGH;
}
