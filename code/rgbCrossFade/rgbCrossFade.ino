const boolean COMMON_ANODE = false; 

const int RGB_RED_PIN = 6;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 3;
const int DELAY_MS = 20; // delay in ms between changing colors
const int MAX_COLOR_VALUE = 255;

const int FADE_STEP = 5;

enum RGB {
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};

int rgbLedValues[] = {255, 0, 0}; // Red, Green, Blue
enum RGB currentFadingUpColor = GREEN;
enum RGB currentFadingDownColor = RED;

void setup() {
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600); 
  Serial.println("Red, Green, Blue");

  // Set initial color
  setColor(rgbLedValues[RED], rgbLedValues[GREEN], rgbLedValues[BLUE]);
  delay(DELAY_MS);
}

void loop() {
  fadeUp();
  fadeDown();
  setColor(rgbLedValues[RED], rgbLedValues[GREEN], rgbLedValues[BLUE]);
  delay(DELAY_MS);
}

void fadeUp() {
  rgbLedValues[currentFadingUpColor] += FADE_STEP;

  boolean isOutOfBounds = rgbLedValues[currentFadingUpColor] >= MAX_COLOR_VALUE;  // if u dont do >= instead of >, red kinda doesnt show up
  if (!isOutOfBounds) return;

  rgbLedValues[currentFadingUpColor] = MAX_COLOR_VALUE;
  currentFadingUpColor = getNextColor(currentFadingUpColor);
}

void fadeDown() {
  rgbLedValues[currentFadingDownColor] -= FADE_STEP;

  boolean isOutOfBounds = rgbLedValues[currentFadingUpColor] <= 0;  // if u dont do <= instead of <, red kinda doesnt show up
  if (!isOutOfBounds) return;

  rgbLedValues[currentFadingDownColor] = 0;
  currentFadingDownColor = getNextColor(currentFadingDownColor);
}


int getNextColor(int color) {
  int nextColor = (int) color + 1;

  bool isOutOfIndex = nextColor > (int) BLUE;
  if (isOutOfIndex) nextColor = RED;

  return nextColor;
}

void setColor(int red, int green, int blue) {
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(blue);

  // If a common anode LED, invert values
  if (COMMON_ANODE) {
    red = MAX_COLOR_VALUE - red;
    green = MAX_COLOR_VALUE - green;
    blue = MAX_COLOR_VALUE - blue;
  }
  
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}