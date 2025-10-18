// MIDINet - Print

// ============================================================================================== //

#ifndef PRINT_H
#define PRINT_H

// Include
#include <Arduino_LED_Matrix.h>
#include <ArduinoGraphics.h>

// Function Declarations

void printToLEDMatrix(ArduinoLEDMatrix &matrix, String message);
void printToTerminal(String message);
void print(ArduinoLEDMatrix &matrix, String message);

#endif