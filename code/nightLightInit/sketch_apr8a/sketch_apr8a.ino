// Author: Ryan Le
// Used code from CITE SOURCES HERE



const int LED_OUTPUT_PIN = 3;
const int PHOTOCELL_INPUT_PIN = A0;

const int MIN_PHOTOCELL_VAL = 500; // turn on LED
const int MAX_PHOTOCELL_VAL = 900; // fully on value

const boolean PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER = true; // set false if photocell is R1

int currentMode = 1;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
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


}


void updateMode() {
  
}

void updateRotation() {

}


void updatePhotocell() {
  // Read the photo-sensitive resistor value. If you have the photocell resistor hooked 
  // up as Rtop in the voltage divider (that is, one leg of the photocell is connected to 5V), 
  // then higher values correspond to brightness. If you have the photocell hooked up as Rbottom 
  // in the voltage divider (that is, one leg of the photocell is connected to GND), then
  // higher values correspond to darkness.
  int photocellVal = analogRead(PHOTOCELL_INPUT_PIN);

  // Remap the value for output. 
  int ledVal = map(photocellVal, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  ledVal = constrain(ledVal, 0, 255);

  if (PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER == false){
    // We need to invert the LED (it should be brighter when environment is darker)
    // This assumes the photocell is Rtop in the voltage divider
    ledVal = 255 - ledVal;
  }

  // Print the raw photocell value and the converted led value (e,g., for Serial 
  // Console and Serial Plotter)
  Serial.print(photocellVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(100); //read from the photocell ~10 times/sec

}
