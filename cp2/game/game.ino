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

// buttons
int RIGHT_BUTTON_PIN = 13;
int LEFT_BUTTON_PIN = 8;
int buttonPins[] = {LEFT_BUTTON_PIN, RIGHT_BUTTON_PIN}; 
int buttonAmount = *(&buttonPins + 1) - buttonPins;

int buttonStatesValues[] = {LOW, LOW};
int previousRawButtonValues[] = {LOW, LOW};
int debouncedButtonValues[] = {LOW, LOW};
unsigned long buttonStateChangeTimestamps[] = {0, 0};
const int DEBOUNCE_WINDOW = 0; // in milliseconds

// menu
enum menuButton {
  play,
  score,
  length,
};


menuButton currentState = play;
boolean hasNotMenuMoverPressedRecently = true;


int currentGame = -1;

void setup(){
  Serial.begin(9600);
  pinMode(RIGHT_BUTTON_PIN, INPUT);
  
  // Initialize the display. If it fails, print failure to Serial
  // and enter an infinite loop
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
}


// menu;
void loop(){
  updateButtonStates();

  // Serial.println(getButtonValue(RIGHT_BUTTON_PIN));

  switch (currentGame) {
    case 0:
        // Code for game 0
        break;
    case 1:
        displayTest();
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

}

int getButtonValue(int PIN_NUMBER) {
  
  for (int i = 0; i < buttonAmount; i++) {
    if (buttonPins[i] == PIN_NUMBER) return debouncedButtonValues[i];
  }

  return -1;

}

void displayMenu() {
  display.clearDisplay();
  
  if (getButtonValue(LEFT_BUTTON_PIN) == HIGH && currentState == play) {
    currentGame = getRandomGame();
    return;
  }

  String title = "TITLE";
  displayTextCenter(title, 3, 0, -16);
  displayMenuButtons();
  display.display();
}

void displayTest() {
  display.clearDisplay();
  displayTextCenter("test", 3, 0, -16);
  display.display();


}

int getRandomGame() {
  return 1;
}

void updateButtonStates() {
  
  for (int i = 0; i < buttonAmount; i++) {
    int rawButtonValue = digitalRead(buttonPins[i]);
    if(rawButtonValue != previousRawButtonValues[i]) buttonStateChangeTimestamps[i] = millis();
    
    unsigned long difference = millis() - buttonStateChangeTimestamps[i];

    if(difference >= DEBOUNCE_WINDOW) debouncedButtonValues[i] = rawButtonValue;
    previousRawButtonValues[i] = rawButtonValue;
  }
}


// menu
// enum menuButton {
//   play,
//   score,
//   length,
// };


void displayMenuButtons() {

  String menuOptions[] = {"play", "score"};
  int menuButtonLength = *(&menuOptions + 1) - menuOptions;

  int xPositions[] = {3, 68};
  int yPositions[] = {60, 60};
  int textSize = 2;

  boolean isButtonPressed = getButtonValue(RIGHT_BUTTON_PIN) == HIGH;
  if (isButtonPressed && hasNotMenuMoverPressedRecently) {
    currentState = (menuButton) ((currentState + 1) % length);

    if (currentState == length) {
      currentState = play; 
    }

    // Serial.println()
    hasNotMenuMoverPressedRecently = false;
  } else if (!isButtonPressed) {
    hasNotMenuMoverPressedRecently = true;
  }


  for (int i = 0; i < menuButtonLength; i++) {
    displayText(menuOptions[i], textSize, xPositions[i], yPositions[i]);
    if ((menuButton) i == currentState) {
      drawUnderline(menuOptions[i], xPositions[i], yPositions[i]);
    }
  }

  // displayTextCenter("play", 2, -30, 15);

}

void drawUnderline(String text, int xPosition, int yPosition) {
  int16_t x;
  int16_t y;
  uint16_t textWidth;
  uint16_t textHeight;

  display.getTextBounds(text, 0, 0, &x, &y, &textWidth, &textHeight);

  int offset = 2;
  display.drawFastHLine(xPosition - 1, yPosition + offset, textWidth, WHITE);


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





