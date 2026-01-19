// MIDINET - Display

// ============================================================================================== //

#ifndef DISPLAY_H
#define DISPLAY_H

// ============================================================================================== //
// Include

#include <vector>

// ============================================================================================== //
// Constants

#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   32    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin #
#define SCREEN_ADDRESS  0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(text);
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
  private:
    std::vector<String> _options;
    int _numberOfOptions;

  public:

    VerticalListMenu(std::vector<String> options) : _options(options) {
        _numberOfOptions = _options.size();
    }

    void display(int currentSelectionIndex) {
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
                writeToLine(1, ">" + displayText);
            } else {
                writeToLine(i + 1, " " + displayText);
            }
        }
    }
};

// ============================================================================================== //

#endif