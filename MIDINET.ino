// MIDINET

// ============================================================================================== //
// Imports

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>

// ============================================================================================== //
// Constants

// Display
#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   32    // OLED display height, in pixels
#define OLED_RESET      -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Rotary Encoder
#define CLK D5
#define DT  D6
#define SW  D3
int counter = 0;
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;
bool buttonPressed = false;

// WiFi
#define MAX_NETWORKS  20
String networkList[MAX_NETWORKS];
int networksFound = 0;

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
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastStateCLK = digitalRead(CLK);

  // Check Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.setTextWrap(false);

  // Main Screen
  mainScreen();
}

// ============================================================================================== //
// Loop

void loop() {
}

// ============================================================================================== //
// Rotary Encoder Rotation & Press

void handleEncoderRotation() {
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(DT) != currentStateCLK) {
      counter--;
    } else {
      counter++;
    }
    Serial.print("Count: "); Serial.println(counter);
  }
  lastStateCLK = currentStateCLK;
}

void handleEncoderPress() {
  int btnState = digitalRead(SW);

  if (btnState == LOW) {
    // If 50ms have passed since the last press, it's a real click
    if (millis() - lastButtonPress > 50) {
      buttonPressed = true;
    }
    lastButtonPress = millis(); // Update the last press time
  }
}

// ============================================================================================== //
// Display Text Functions

void writeToLine(int line, String text) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, line * 8);
  display.println(text);
}

void headerText(String text) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(text);
}

// ============================================================================================== //
// String Format Helper Function

String fmt(const char* format, ...) {
  char buffer[128];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  return String(buffer);
}

// ============================================================================================== //
// WiFi Functions

void performWiFiScan() {
  Serial.println("Scanning...");
  networksFound = 0; 
  
  int n = WiFi.scanNetworks(); 
  
  if (n > 0) {
    networksFound = (n > MAX_NETWORKS) ? MAX_NETWORKS : n;

    for (int i = 0; i < networksFound; i++) {
      networkList[i] = WiFi.SSID();
    }
  }
  WiFi.scanDelete();
}

// ============================================================================================== //
// Vertical List Menu

void verticalListMenu(String options[], int optionsLength) {
  // We treat the list as if it has one extra "blank" slot
  int virtualLength = optionsLength + 1;

  for (int i = 0; i < 3; i++) {
    // Calculate the index based on the virtual length
    int virtualIndex = ((counter + i) % virtualLength + virtualLength) % virtualLength;
    
    String displayText;
    
    // If we are at the very last index, make it a blank line
    if (virtualIndex == optionsLength) {
      displayText = "=========="; 
    } else {
      displayText = options[virtualIndex];
    }

    // Now draw the line
    if (i == 0) {
      writeToLine(1, ">" + displayText);
    } else {
      writeToLine(i + 1, " " + displayText);
    }
  }
}

// ============================================================================================== //
// Main Screen

void mainScreen() {
  bool loop_flag = true;
  buttonPressed = false;

  while (loop_flag) {
    // ================================================== //
    // Display
    display.clearDisplay();

    // Header
    String networkName = "SomeNetwork";
    int signalStrength = 25;
    headerText(fmt("Main %s %d", networkName.c_str(), signalStrength));

    display.display();

    // ================================================== //
    // User Input
    handleEncoderRotation();
    handleEncoderPress();

    if (buttonPressed) {
      optionsMenu();
    }
  }
}

// ============================================================================================== //
// Options Menu

void optionsMenu() {
  bool loop_flag = true;
  buttonPressed = false;
  counter = 0;

  String options[4] = {
    "Network",
    "Option 2",
    "Option 3",
    "Option 4"
  };
  int optionsLength = sizeof(options) / sizeof(options[0]);

  while (loop_flag) {
    // ================================================== //
    // Display
    display.clearDisplay();
    headerText("Options");
    verticalListMenu(options, optionsLength);
    display.display();

    // ================================================== //
    // User Input
    handleEncoderRotation();
    handleEncoderPress();

    if (buttonPressed) {
      int selection = counter % optionsLength;
      if (selection == 0) {
        networkMenu();
      }

      buttonPressed = false;
    }
  }
}

// ============================================================================================== //
// Network Menu

void networkMenu() {
  bool loop_flag = true;
  buttonPressed = false;
  networksFound = 0;

  while (loop_flag) {
    // ================================================== //
    // Scan For WiFi
    if (networksFound == 0) {
      performWiFiScan();
    }

    // ================================================== //
    // Display
    display.clearDisplay();
    headerText("Network Menu");
    verticalListMenu(networkList, networksFound);
    display.display();

    // ================================================== //
    // User Input
    handleEncoderRotation();
    handleEncoderPress();

    if (buttonPressed) {
      int selection = counter % networksFound;
      Serial.println(fmt("%s", networkList[selection]));
    }
  }
}