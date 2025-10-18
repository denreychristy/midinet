// MIDINet - Setup Serial

// Include
#include <Arduino.h>

void setupSerial() {
	Serial.begin(9600);
	while (!Serial);
}