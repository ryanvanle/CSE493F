/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

// timer
unsigned long startTime = 0;
unsigned long duration = 0;
boolean isTimerRunning = false;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();

}

void loop() {
  if (!isTimerRunning) {
    setAndStartTimer(8);
  }
  
  display.clearDisplay();
  displayText(String(remainingTimerTimeSeconds()), 1, 20, 20);
  displayUI();
  display.display();


  if (checkTimerElasped()) {
    clearTimer();
  }
}

boolean pulse = false;

void displayTimer() {

    int radius = 3;
    int xPos = 3;
    int yPos = 59;
    long timeMS = remainingTimerTimeMS();

    int speed = 80;

    long longLineLength = map(timeMS, 3000, duration, 0, speed);
    long downLineLength = map(timeMS, 2000, duration, 0, speed);
    long shortLineLength = map(timeMS, 1000, duration, 0, speed);
    long upLineLength = map(timeMS, 0, duration, 0, speed );

    Serial.print("long: ");
    Serial.println(longLineLength);

    if (longLineLength > 0) {
      display.drawCircle(xPos, yPos, radius, WHITE); // circle
      display.drawFastVLine(xPos, yPos - radius - 2, 2, WHITE);
      display.drawFastHLine(xPos, yPos - radius - 2, 6, WHITE);
      display.drawFastVLine(xPos + 6, yPos - radius - 2, 2 + radius + radius, WHITE);

      display.drawFastHLine(xPos + 6, yPos + radius, longLineLength, WHITE);
      drawTimerTrailEffect(xPos + 6 + longLineLength, yPos + radius);

    } else if (downLineLength > 0) {
      display.drawCircle(xPos, yPos, radius, WHITE); // circle
      display.drawFastVLine(xPos, yPos - radius - 2, 2, WHITE);
      display.drawFastHLine(xPos, yPos - radius - 2, 6, WHITE);
      display.drawFastVLine(xPos + 6, yPos - radius - 2, downLineLength, WHITE);
      drawTimerTrailEffect(xPos + 6, yPos - radius - 2 + downLineLength);

    } else if (shortLineLength > 0) {
      display.drawCircle(xPos, yPos, radius, WHITE); // circle
      display.drawFastVLine(xPos, yPos - radius - 2, 2, WHITE);
      display.drawFastHLine(xPos, yPos - radius - 2, shortLineLength, WHITE);
      drawTimerTrailEffect(xPos + shortLineLength, yPos - radius - 2);

    } else if (upLineLength > 0) {
      display.drawCircle(xPos, yPos, radius, WHITE); // circle
      display.drawFastVLine(xPos, yPos - radius - upLineLength, upLineLength - 1, WHITE);
      drawTimerTrailEffect(xPos, yPos - radius - upLineLength - 1);

    } else {
      playTimerTransition(xPos, yPos);
    }


    pulse = !pulse;

}


void playTimerTransition(int xPos, int yPos) {
  // from the adafruit examples 
  for(int16_t i = 0; i < display.width(); i += 3) {
    display.drawCircle(xPos, yPos, i, SSD1306_WHITE);
    display.display();
  }

}

void drawTimerTrailEffect(int xPos, int yPos) {
  
  int triangleAmount = 2;
  int radius = 4;



  for (int i = 0; i < triangleAmount; i++) {
    int x0 = xPos + random(-radius, radius);
    int y0 = yPos + random(-radius, radius);

    int x1 = xPos + random(-radius, radius);
    int y1 = yPos + random(-radius, radius);
    
    int x2 = xPos + random(-radius, radius);
    int y2 = yPos + random(-radius, radius);

    display.drawTriangle(x0, y0, x1, y1, x2, y2, WHITE);    
  }


}

void displayTextCenter(String text, uint8_t textSize, int16_t xOffset, int16_t yOffset) {
  int16_t x;
  int16_t y;
  uint16_t textWidth;
  uint16_t textHeight;

  display.setTextSize(textSize);
  display.setTextColor(WHITE, BLACK);
  display.getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
  centerCursorWithText(xOffset, yOffset, textWidth, textHeight);
  display.print(text);
}

void displayText(String text, uint8_t textSize, int16_t xPosition, int16_t yPosition) {
  int16_t x;
  int16_t y;
  uint16_t textWidth;
  uint16_t textHeight;

  display.setTextSize(textSize);
  display.setTextColor(WHITE, BLACK);
  display.getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);
  display.setCursor(xPosition, yPosition - textHeight);
  display.print(text);
}

void centerCursorWithText(int16_t xOffset, int16_t yOffset, uint16_t textWidth, uint16_t textHeight) {
  display.setCursor(display.width() / 2 - textWidth / 2 + xOffset, display.height() / 2 - textHeight / 2 + yOffset);
}

void displayUI() {
  displayHearts();
  displayScore();
  displayInputIcon(false);
  displayTimer();
}


void displayInputIcon(boolean isIntro) {
  return;
}

void displayHearts() {
  display.setCursor(0, 1);
  display.setTextSize(1);

  int heartIndex = 3;

  int currentLives = 5;
  for (int i = 0; i < currentLives; i++) {
    display.write(heartIndex);
  }
}

void displayScore() {

  int currentScore = 10;
  int xOffset;

  if (currentScore < 10) {
    xOffset = 120;
  } else if (currentScore < 100) {
    xOffset = 115;
  } else if (currentScore < 1000) {
    xOffset = 110;
  } else {
    xOffset = 103;
  }

  displayText(String(currentScore), 1, xOffset, 10);
}



boolean setTimer(int seconds) {  
  if (isTimerRunning) return false;

  duration = ((unsigned long) seconds) * 1000; // Convert seconds to milliseconds
  return true;
}

boolean setAndStartTimer(int seconds) {
  return setTimer(seconds) && startTimer();
}

boolean startTimer() {
  if (isTimerRunning) return false;

  startTime = millis();
  isTimerRunning = true;
  
  return isTimerRunning;
}

boolean clearTimer() {
  if (!isTimerRunning) return false;

  startTime = 0;
  duration = 0;
  isTimerRunning = false;

  return true;
}

boolean checkTimerElasped() {
  boolean hasElaspedTime = millis() - startTime >= duration;
  return hasElaspedTime;
}

long remainingTimerTimeMS() {
  if (checkTimerElasped()) return 0;
  return duration - (millis() - startTime);
}

int remainingTimerTimeSeconds() {
  if (checkTimerElasped()) return 0;
  return (int) ((duration - (millis() - startTime)) / 1000);
}


