// MIDINet - Main

// ============================================================================================== //

// Include
#include <string>
#include <Arduino_LED_Matrix.h>
#include <WiFiS3.h>
#include <MIDI.h>
#include "../lib/setupSerial.h"
#include "../lib/setupMatrix.h"
#include "../lib/print.h"
#include "../lib/setupWiFi.h"

// Variable declarations
ArduinoLEDMatrix matrix;
MIDI_CREATE_DEFAULT_INSTANCE();

// ============================================================================================== //

// Initital Set Up Function
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(21, OUTPUT);
	digitalWrite(21, LOW);
	//setupSerial();
	
	MIDI.begin();

	setupMatrix(matrix);
	//setupWiFi(matrix);
}

// ============================================================================================== //

// Main Loop
void loop() {
	// Send a MIDI Note On message on channel 1
	// Parameters: Note Number, Velocity, Channel
	MIDI.sendNoteOn(60, 127, 1);

	// Wait for a short period
	delay(500);

	// Send a MIDI Note Off message on channel 1
	MIDI.sendNoteOff(60, 0, 1);

	// Wait again
	delay(500);
}