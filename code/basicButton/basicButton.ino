const int INPUT_BUTTON_PIN = 2;
const int OUTPUT_LED_PIN = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_BUTTON_PIN, INPUT);
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonValue = digitalRead(INPUT_BUTTON_PIN);
  
  boolean isPressed = buttonValue == HIGH;

  if (isPressed) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }
  digitalWrite(OUTPUT_LED_PIN, buttonValue);

  delay(5);
}
