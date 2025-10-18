// MIDINet - Setup Matrix

// Include
#include <Arduino_LED_Matrix.h>

void setupMatrix(ArduinoLEDMatrix &matrix) {
	matrix.begin();
	matrix.textFont(Font_5x7);
	matrix.textScrollSpeed(50);
	matrix.stroke(0xFF, 0xFF, 0xFF);
}