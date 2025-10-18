// MIDINet - UDP To MIDI

// Include
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// Variable Declarations
WiFiUDP udp;
unsigned int localPort = 5005;
int status = udp.begin(localPort);
char packetBuffer[1000];

void udpToMIDI() {
	int packetSize = udp.parsePacket();
	if (packetSize > 0) {
		int len = udp.read(packetBuffer, packetSize);
		if (len > 0) { packetBuffer[len] = 0; }

		const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
		StaticJsonDocument<capacity> udpjson;
		DeserializationError error = deserializeJson(udpjson, packetBuffer);

		if (error) {
			//Serial.print(F("deserializeJson() failed: "));
			//Serial.println(error.f_str());
			return;
		}

		const String type = udpjson["type"];
		const byte channel = udpjson["channel"];
		const byte number = udpjson["number"];
		const byte value = udpjson["value"];
		String result = type + " " + String(channel) + " " + String(number) + " " + String(value);
		//printToTerminal(result);

		//if (type == "note on") {
		//	midiDevice.sendNoteOn(60, 127, 1);
		//	delay(500);
		//	midiDevice.sendNoteOff(60, 0, 1);
		//}
	}
}