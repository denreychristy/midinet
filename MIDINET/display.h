// MIDINET - Display

// ============================================================================================== //

#ifndef DISPLAY_H
#define DISPLAY_H

// ============================================================================================== //
// Include

#include <vector>

#include "math.h"
#include "rotary_encoder.h"

// ============================================================================================== //
// Constants

#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   32    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin #
#define SCREEN_ADDRESS  0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const String charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};:'\",.<>/?\\|~ ";
const int charsetLen = charset.length();
int upArrowIndex = 67;

int scrollThreshold = 128; // number of pixels for text width that necessitates scrolling
int scrollStartDelay = 2000;  // time in milliseconds before scrolling text starts to move
int textScrollSpeed = 200;  // time in milliseconds for scrolling text to move to the next character

// ============================================================================================== //
// Write To Line

void writeToLine(int line, String text) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, line * 8);
  display.println(text);
}

// ============================================================================================== //
// Header Text

void headerText(String text) {
  static int offset = 0;
  static unsigned long scrollDelayTime = 0;
  static unsigned long lastMove = 0;
  static String lastText = ""; // Stores the text from the previous call

  // Check if the menu text has changed
  if (text != lastText) {
    offset = 0;                 // Reset scroll position
    scrollDelayTime = millis(); // Reset delay timer
    lastMove = millis();        // Reset timer
    lastText = text;            // Update the "remembered" text
  }

  int displayWidth = 21; // Max characters at size 1
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // For short text, just print the line
  if (text.length() <= displayWidth) {
    display.println(text);
  }
  
  // Wait for the scroll delay start before moving
  else if ((millis() - scrollDelayTime) < scrollStartDelay) {
    display.println(text);
  }
  
  else {  
    // Ensure we don't slice past the end of the string
    int endIdx = offset + displayWidth;
    String window = text.substring(offset, endIdx);
    
    display.println(window);

    if (millis() - lastMove > textScrollSpeed) {
      offset++;
      if (offset >= text.length()) {
        offset = 0;
        scrollDelayTime = millis();
      }
      lastMove = millis();
    }
  }
}

// ============================================================================================== //
// String Format Helper Function

String format(const char* format, ...) {
  char buffer[128];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  return String(buffer);
}

// ============================================================================================== //
// Get Text Width

int getTextWidth(String text) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return w;
}

// ============================================================================================== //
// Build String From Vector

// String buildStringFromVector(std::vector<int> &vec, int flashIndex, bool flash) {
//   String result = "";
//   for (int i = 0; i < vec.size(); i++) {
//     int charIndex = vec[i] % charsetLen;
//     if (i == flashIndex && !flash) {
//       result += " ";
//     } else {
//       result += charset[charIndex];
//     }
//   }
//   return result;
// }

// ============================================================================================== //
// Vertical List Menu

class VerticalListMenu {
  // ================================================== //
  private:
    String _header;
    std::vector<String> _options; // vector of menu item names
    int _numberOfOptions; // length of _options
    std::vector<int> _lineLengthsInPixels;  // the display width in pixels of each menu item name
    unsigned long _scrollDelayTime; // the start time for the delay before it starts to scroll
    int _currentScrollIndex;  // the current scroll position of the currently selected menu item
    unsigned long _lastScrollUpdateTime; // the time that the scroll function last moved
    int _lastSelected;  // the most recently selected menu item
    unsigned long _lastSelectedTime;  // when the most recently selected menu item was selected

  // ================================================== //
  public:
    // ================================================== //
    VerticalListMenu(String header, std::vector<String> options) {
        _header = header;
        _options = options;
        _numberOfOptions = _options.size();
        _lineLengthsInPixels = {};
        for (int i = 0; i < _numberOfOptions; i++) {
          _lineLengthsInPixels.push_back(getTextWidth(options[i]));
        }
        _currentScrollIndex = 0;
        _lastScrollUpdateTime = millis();
        _lastSelected = 0;
        _lastSelectedTime = millis();
    }

    // ================================================== //
    void update(int currentSelectionIndex) {
      // Update fields if different menu item is selected
      if (_lastSelected != currentSelectionIndex) {
        _currentScrollIndex = 0;
        _scrollDelayTime = millis();
        _lastScrollUpdateTime = millis();
        _lastSelected = currentSelectionIndex;
        _lastSelectedTime = millis();
      }

      // Quit if current line isn't long enough to scroll
      int lineLength = _lineLengthsInPixels[currentSelectionIndex] + getTextWidth(">");
      if (lineLength <= scrollThreshold) { return; }
      
      // Quit if it isn't time to scroll yet
      if ((millis() - _scrollDelayTime) < scrollStartDelay) { return; }
      if ((millis() - _lastScrollUpdateTime) < textScrollSpeed) { return; }
      
      // Scroll
      int charactersInThisLine = _options[currentSelectionIndex].length();
      _currentScrollIndex = (_currentScrollIndex + 1) % (charactersInThisLine - 1);
      if (_currentScrollIndex == 0) {
        _scrollDelayTime = millis();  // restart scroll delay timer on wrap
      }
      _lastScrollUpdateTime = millis();
    }

    // ================================================== //
    void display(int currentSelectionIndex) {
        update(currentSelectionIndex);
        headerText(_header);

        // ================================================== //
        // Menu Items
        int virtualLength = _numberOfOptions + 1;

        for (int i = 0; i < 3; i++) {
            int virtualIndex = ((currentSelectionIndex + i) % virtualLength + virtualLength) % virtualLength;
            
            String displayText;
            if (virtualIndex == _numberOfOptions) {
                displayText = ""; 
            } else {
                displayText = _options[virtualIndex];
            }

            if (i == 0) {
                writeToLine(1, ">" + displayText.substring(_currentScrollIndex));
            } else {
                writeToLine(i + 1, " " + displayText);
            }
        }
    }
};

// ============================================================================================== //
// Text Entry

class TextEntry {
  // ================================================== //
  private:
    String _header;
    String _userText;
    int _currentEditIndex;
    bool _editingCharacter;
    long unsigned _flashTimer;

  // ================================================== //
  public:
    // ================================================== //
    TextEntry(String header) {
      _header = header;
      _userText = "a";
      _currentEditIndex = 0;
      _editingCharacter = true;
    }

    // ================================================== //
    String getUserText() {
      _userText.trim();
      return _userText;
    }

    // ================================================== //
    void update(Encoder &encoder) {
      if (encoder.wasQuickPressed) {
        encoder.wasQuickPressed = false; 
        _editingCharacter = !_editingCharacter;
        return;
      }

      if (encoder.hasBeenTurned) {
        if (_editingCharacter) {
          char currentCharacter = _userText[_currentEditIndex];
          int currentCharacterIndex = charset.indexOf(currentCharacter);
          
          int newIndex;
          if (encoder.hasBeenIncreased) {
            newIndex = (currentCharacterIndex + 1) % charsetLen;
          } else {
            newIndex = (currentCharacterIndex - 1 + charsetLen) % charsetLen;
          }
          
          _userText[_currentEditIndex] = charset[newIndex];
        } 
        else {
          if (encoder.hasBeenIncreased) {
            _currentEditIndex++;
            if (_currentEditIndex >= (int)_userText.length()) {
              _userText += "a"; 
            }
          } else {
            _currentEditIndex = max(0, _currentEditIndex - 1);
          }
        }
        encoder.hasBeenTurned = false;
      }
    }

    // ================================================== //
    void display() {
      headerText(_header);

      char currentSelectedChar = _userText[_currentEditIndex];
      int currentIdx = charset.indexOf(currentSelectedChar);
      int prevIdx = (currentIdx - 1 + charset.length()) % charset.length();
      char charAbove = charset[prevIdx];
      int nextIdx = (currentIdx + 1) % charset.length();
      char charBelow = charset[nextIdx];
      String padding = "";
      
      for (int i = 0; i < _currentEditIndex; i++) {
        padding += " ";
      }

      // Line 1
      if (_editingCharacter) {
        writeToLine(1, padding + charAbove);
      }

      // Line 2 - User Entered Text
      if (_editingCharacter) {
        bool flash = (millis() / 500) % 2;
        String editChar = flash ? String(currentSelectedChar) : " ";
        String line2 = _userText.substring(0, _currentEditIndex) + editChar + _userText.substring(_currentEditIndex + 1);
        writeToLine(2, line2);
      }
      else {
        writeToLine(2, _userText);
      }

      // Line 3
      if (_editingCharacter) {
        writeToLine(3, padding + charBelow);
      }
      else {
        writeToLine(3, padding + "^");
      }
    }
};

// ============================================================================================== //

#endif