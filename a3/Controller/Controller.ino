
// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

const int BUTTON_INPUT_PIN = 5;
const int LED_OUTPUT_PIN = 3;
const int DEBOUNCE_WINDOW = 40; // in milliseconds

int _prevRawButtonVal = LOW;
int _debouncedButtonVal = LOW;
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the input pin was toggled



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
  buttonData();
  sendData();
  delay(100);
}

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
