// MIDINet - Setup WiFi

// Include
#include <ArduinoGraphics.h>
#include <WiFiS3.h>
#include "../lib/print.h"

// Variable Declarations
const char* ssid = "Denrey&Lisa";
const char* password = "daxtermk";

// Connect to WiFi
void setupWiFi(ArduinoLEDMatrix &matrix) {
	print(matrix, "Connecting...");

	int networkCount = WiFi.scanNetworks();

	if (networkCount == 0) {
		print(matrix, "No WiFi networks detected!");
	}
	for (int i = 0; i < networkCount; i++) {
		String result = String(i + 1) + ": " + WiFi.SSID(i);
		print(matrix, result);
	}

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		print(matrix, ".");
	}

	print(matrix, "WiFi connected!");
	print(matrix, "SSID: " + String(ssid));
	delay(1000);
	IPAddress ip = WiFi.localIP();
	print(matrix, "IP address: " + ip.toString());
}