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
// Instantiate SSD1306 driver display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(){
  Serial.begin(9600);
  
  // Initialize the display. If it fails, print failure to Serial
  // and enter an infinite loop
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the display
  display.clearDisplay();
}

int i = 0;
int increment = 1;

int currentGame = -1;

// menu;
void loop(){


switch (currentGame) {
    case 0:
        // Code for game 0
        break;
    case 1:
        // Code for game 1
        break;
    case 2:
        // Code for game 2
        break;
    case 3:
        // Code for game 3
        break;
    case 4:
        // Code for game 4
        break;
    case 5:
        // Code for game 5
        break;
    default:
        displayMenu();
        // Code for default case
        break;
  }


  // Empty on purpose to make a point about how graphic content persists
  // on screen
  // display.clearDisplay();
  // display.fillCircle(i, 20, 10, SSD1306_WHITE);
  // display.display();
  // i += increment;

  // if (i > 128 || i < 0) {
  //   increment = increment * -1;
  // }
}



enum menuButton {
  play,
  score,
  length,
};

menuButton currentState = play;

void displayMenu() {
  display.clearDisplay();

  const char title[] = "TITLE";
  displayTextCenter(title, 3, 0, -16);
  displayMenuButtons();

  display.display();
}


void displayMenuButtons() {

  // play button
  // const char

  displayTextCenter("play", 2, -30, 15);
  // displayTextCenter("play", 2, -30, 15);

}




void displayTextCenter(const char text[], uint8_t textSize, int16_t xOffset, int16_t yOffset) {
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

void centerCursorWithText(int16_t xOffset, int16_t yOffset, uint16_t textWidth, uint16_t textHeight) {
  display.setCursor(display.width() / 2 - textWidth / 2 + xOffset, display.height() / 2 - textHeight / 2 + yOffset);
}





