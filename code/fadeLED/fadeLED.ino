const int LED_OUTPUT_PIN = 3;
const int MAX_ANALOG_OUT = 255;
const int DELAY_MS = 50;

// globals
int fadeAmount = 5;
int currentBrightness = 0;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  updateLight();
}

void updateLight() {
  analogWrite(LED_OUTPUT_PIN, currentBrightness);

  currentBrightness += fadeAmount;
  bool isOutOfBounds = currentBrightness <= 0 || currentBrightness >= MAX_ANALOG_OUT;
  if (isOutOfBounds) fadeAmount = -fadeAmount;
  
  delay(DELAY_MS);
}