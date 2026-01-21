// MIDINET - Rotary Encoder

// ============================================================================================== //

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

// ============================================================================================== //
// Encoder Class

class Encoder {
  // ================================================== //
  private:
    int _clk;
    int _dt;
    int _sw;
    int _lastStateCLK;
    int _buttonState;

    // ================================================== //
    void _handleRotation() {
      int currentStateCLK = digitalRead(_clk);

      if (currentStateCLK != _lastStateCLK && currentStateCLK == 1) {
        hasBeenTurned = true;
        if (digitalRead(_dt) != currentStateCLK) {
          value--;
          hasBeenDecreased = true;
          hasBeenIncreased = false;
        }
        else {
          value++;
          hasBeenIncreased = true;
          hasBeenDecreased = false;
        }
      }
      
      else {
        hasBeenTurned = false;
        hasBeenIncreased = false;
        hasBeenDecreased = false;
      }
      
      _lastStateCLK = currentStateCLK;
    }

    // ================================================== //
    void _handlePress() {
      _buttonState = digitalRead(_sw);

      wasQuickPressed = false;
      wasDoublePressed = false;
      wasLongPressed = false;

      if (_buttonState == LOW) { // If the button is pressed
        if (!isPressed) { // If it was off before this iteration
          lastPressed = millis(); // set the lastPressed time
        }
        else { // If it was already being pressed before this iteration
          // Do nothing
        }
        isPressed = true; // set isPressed to true
      }

      else { // If the button is not pressed
        if (isPressed) { // if it was on before this iteration
          unsigned long previousLastReleased = lastReleased; // move the previous lastReleased value
          lastReleased = millis(); // set the lastReleased time
          if (lastReleased - lastPressed >= 3000) { // if the button was pressed for over 3 seconds
            wasLongPressed = true; // set wasLongPressed to true
          }
          else { // In the event of a quick press
            if (lastReleased - previousLastReleased <= 1000) { // if the button is pressed twice within 1 second
              wasDoublePressed = true;
            }
            else {
              wasQuickPressed = true; // set wasQuickPressed to true
            }
          }
        }
        else { // If it was not pressed before
          // Do nothing
        }
        isPressed = false; // set isPressed to false
      }
    }

  // ================================================== //
  public:
    int value = 0;
    bool isPressed = false;
    bool wasQuickPressed = false;
    bool wasDoublePressed = false;
    bool wasLongPressed = false;
    unsigned long lastPressed = 0;
    unsigned long lastReleased = 0;
    bool hasBeenTurned = false;
    bool hasBeenIncreased = false;
    bool hasBeenDecreased = false;

    // ================================================== //
    Encoder(int clk, int dt, int sw) : _clk(clk), _dt(dt), _sw(sw) {}
  
    // ================================================== //
    void begin() {
      pinMode(_clk, INPUT);
      pinMode(_dt, INPUT);
      pinMode(_sw, INPUT_PULLUP);
      _lastStateCLK = digitalRead(_clk);
    }

    // ================================================== //
    void reset() {
      value = 0;
    }

    // ================================================== //
    void update() {
      _handlePress();
      _handleRotation();
    }

    // ================================================== //
    String toString() {
      String result = "";
      result += "Encoder: ";
      result += "value: " + String(value) + " | ";
      result += "isPressed: " + String(isPressed) + " | ";
      result += "wasQuickPressed: " + String(wasQuickPressed) + " | ";
      result += "wasDoublePressed: " + String(wasDoublePressed) + " | ";
      result += "wasLongPressed: " + String(wasLongPressed) + " | ";
      result += "lastPressed: " + String(lastPressed) + " | ";
      result += "lastReleased: " + String(lastReleased) + " | ";
      result += "hasBeenTurned: " + String(hasBeenTurned) + " | ";
      result += "hasBeenIncreased: " + String(hasBeenIncreased) + " | ";
      result += "hasBeenDecreased: " + String(hasBeenDecreased) + " | ";

      return result;
    }
};

// ============================================================================================== //

#endif