#include "Blinker.h"

Blinker::Blinker(const int pin, const unsigned long blinkInterval)
  :_pin(pin), _interval(blinkInterval)

{
  _state = LOW;
  _lastToggledTimestamp = 0;
  pinMode(_pin, OUTPUT);
}

// methods

/**
* Calculates whether to toggle output state based on the set interval
* Call this function once per loop()
*/
void Blinker::update() {
  unsigned long currentTimestampMs = millis();
  unsigned long timeDifference = currentTimestampMs - _lastToggledTimestamp;
  boolean hasEnoughTimeElapsed = currentTimestampMs - _lastToggledTimestamp >= _interval;

  if (!hasEnoughTimeElapsed) return;

  _lastToggledTimestamp = currentTimestampMs;
  _state = !_state; // flips from high to low since high is 1 and low is 0?
  digitalWrite(_pin, _state);
};
