// MIDINet - Print

// ============================================================================================== //

// Include
#include "../lib/print.h"
#include <Arduino_LED_Matrix.h>
#include <ArduinoGraphics.h>

// ============================================================================================== //

void printToLEDMatrix(ArduinoLEDMatrix &matrix, String message) {
	matrix.beginDraw();
	matrix.beginText(0, 1, 0xFFFFFF);
	matrix.println(message);
	matrix.endText(SCROLL_LEFT);
	matrix.endDraw();
}

// ============================================================================================== //

void printToTerminal(String message) {
	Serial.println(message);
}

// ============================================================================================== //

void print(ArduinoLEDMatrix &matrix, String message) {
	printToTerminal(message);
	printToLEDMatrix(matrix, "     " + message);
}

// ============================================================================================== //