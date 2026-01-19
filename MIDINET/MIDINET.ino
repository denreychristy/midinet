// MIDINET

// ============================================================================================== //
// Imports

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <vector>

#include "display.h"
#include "math.h"
#include "rotary_encoder.h"

// ============================================================================================== //
// Constants

// Display
extern Adafruit_SSD1306 display;

// Encoder
Encoder encoder(D5, D6, D3);

// WiFi
#define MAX_NETWORKS  20
String networkList[MAX_NETWORKS];
int networksFound = 0;
const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};:'\",.<>/?\\|~ ";
const int charsetLen = strlen(charset); // Use strlen to ignore the null terminator
int upArrowIndex = 67;

VerticalListMenu test = VerticalListMenu({"option 1", "option 2", "option 3"});

// ============================================================================================== //
// Setup

void setup() {
  // Begin USB Connection
  Serial.begin(115200);
  delay(500);

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Rotary Encoder
  encoder.begin();

  // Check Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.setTextWrap(false);
}

// ============================================================================================== //
// Loop

void loop() {
  encoder.update();
  
  display.clearDisplay();
  test.display(encoder.value % 3);
  display.display();
}

// ============================================================================================== //
// WiFi Functions

void performWiFiScan() {
  networksFound = 0; 
  
  int n = WiFi.scanNetworks(); 
  
  if (n > 0) {
    networksFound = (n > MAX_NETWORKS) ? MAX_NETWORKS : n;

    for (int i = 0; i < networksFound; i++) {
      networkList[i] = WiFi.SSID(i);
    }
  }
  WiFi.scanDelete();
}